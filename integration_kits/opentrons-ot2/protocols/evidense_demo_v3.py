from opentrons import protocol_api
import os
import sys
from enum import Enum

class dotdict(dict):
    """dot.notation access to dictionary attributes"""
    __getattr__ = dict.get
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__

metadata = {'apiLevel': '2.24', 
            'protocolName': 'eviDense Demo V3',
            'author': 'DaZw'}
            
safety_height = 40
    
class Speed(Enum):
    CUVETTE_PICKUP_DOWN  = 10.0
    CUVETTE_PICKUP_UP    = 10.0
    CUVETTE_MEASURE_DOWN = 30.0
    CUVETTE_MEASURE_UP   = 30.0

class Instrument:
    
    def __init__(self, protocol):
        self.protocol   = protocol
        

        self.workdeck   = { 'sample_plate'    : protocol.load_labware('corning_96_wellplate_360ul_flat',      '5'),
                            'evidense'        : protocol.load_labware('hse_evidense_pilot_right_20ul_tip_v3', '4'),
                            'tiprack_20'      : protocol.load_labware('opentrons_96_filtertiprack_20ul',      '6'),
                          }
        
        self.workdeck   = dotdict(self.workdeck)
        
        self.cuvette_well_index     = 1        
        
        if not protocol.is_simulating():
            path = '/var/lib/jupyter/notebooks/runs/evidense'
            os.makedirs(path, exist_ok=True)            
            self.run = evidense.Run(self.protocol.params.nr_of_std_low, path = path)

        self.pipette20_x1   = protocol.load_instrument('p20_single_gen2', 'left', tip_racks=[self.workdeck.tiprack_20])
        self.cuvette_source = self.workdeck.evidense
        
    def error_handling(self):
        if self.protocol.params.pause_on_error:
            if self.run.verification.failed():
                self.protocol.pause("An error or a warning occurred!")

    def tip_pickup(self):
        self.pipette20_x1.pick_up_tip()

    def tip_drop(self):
        self.pipette20_x1.drop_tip()
    
    def sample_aspirate(self, volume, position, height=1):
        well = self.workdeck.sample_plate.wells()[position]
        self.pipette20_x1.aspirate(volume, well.bottom(height))

    def cuvette_pickup(self):
        well = self.cuvette_source.wells()[self.cuvette_well_index]
        self.cuvette_well_index = self.cuvette_well_index + 1
        self.pipette20_x1.move_to(well.top())
        self.pipette20_x1.move_to(location = well.bottom(), speed = Speed.CUVETTE_PICKUP_DOWN.value, publish = False)
        self.protocol.delay(seconds = 0.5)
        self.pipette20_x1.move_to(location = well.top(),                   speed = Speed.CUVETTE_PICKUP_UP.value, publish = False)
        self.pipette20_x1.move_to(location = well.top(z = safety_height), publish = False)

    def check_if_cuvette_holder_is_empty(self):
        if not self.protocol.is_simulating():
            if not self.run.check_empty():
                self.protocol.pause("Cuvette holder is not empty! Fix it and go ahead.")

    def move_over_cg(self):
        cuvette_guide  = self.workdeck.evidense.well('A14')
        self.pipette20_x1.move_to(location = cuvette_guide.top(z = safety_height), publish = False)
        
    def move_into_cg(self): 
        cuvette_guide  = self.workdeck.evidense.well('A14')
        self.pipette20_x1.move_to(location = cuvette_guide.top(z = safety_height), publish = False)
        self.pipette20_x1.move_to(location = cuvette_guide.top(), publish = False)
        self.check_if_cuvette_holder_is_empty()

        self.pipette20_x1.move_to(location = cuvette_guide.top(), publish = False)
        self.pipette20_x1.move_to(location = cuvette_guide.bottom(), speed = Speed.CUVETTE_MEASURE_DOWN.value, publish = False)
        
    def move_out_of_cg(self):
        cuvette_guide  = self.workdeck.evidense.well('A14')
        self.pipette20_x1.move_to(location = cuvette_guide.top(), speed = Speed.CUVETTE_MEASURE_UP.value, publish = False)
        self.pipette20_x1.move_to(location = cuvette_guide.top(z = safety_height), publish = False)
                
    def comment(self, position):
        row = position % 8
        col = position // 8
        row_map = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
        
        if position < self.protocol.params.nr_of_std_low:
            sample_type = "Standard low"
        else:
            sample_type = "Sample"
    
        return "{}@{}{}".format(sample_type, row_map[row], col+1)
   
    def measure_with_1_channel(self, sample):
        VOLUME = 10.5 #ul
        EXTRA  = 0.5  #ul
        
        self.move_over_cg()
        # Baseline measurement
        if not self.protocol.is_simulating():
            self.run.measure(self.comment(sample))
            self.error_handling()
        
        self.move_into_cg()
        # Air measurement
        if not self.protocol.is_simulating():
            self.run.measure(self.comment(sample))
            self.error_handling()
            
        self.pipette20_x1.dispense(VOLUME)
        # Sample measurement
        if not self.protocol.is_simulating():
            self.run.measure(self.comment(sample))      
            self.error_handling()
            
        self.move_out_of_cg()
        self.pipette20_x1.aspirate(VOLUME + EXTRA)

    def aspirate_from_sample_plate_and_measure(self):
        VOLUME = 12
      
        for position in range(self.protocol.params.nr_of_std_low + self.protocol.params.number_of_samples):
            self.tip_pickup()
            self.sample_aspirate(VOLUME, position)
            self.cuvette_pickup()
            self.measure_with_1_channel(position)
            self.tip_drop()


def run(protocol: protocol_api.ProtocolContext):

    if not protocol.is_simulating():
        sys.path.insert(0,'/var/lib/jupyter/notebooks')
        global evidense
        from hse import evidense
        
    assert protocol.params.nr_of_std_low + protocol.params.number_of_samples <= 96, "The sum of <standard low> + <Number of samples> must be less or equal 96."
        
    i = Instrument(protocol)
  
    if not protocol.is_simulating():
        i.run.device.logging() # delete all log messages until now
        i.run.storage.add_dict("parameters", { 'number_of_samples'  : protocol.params.number_of_samples, 
                                               'nr_of_std_low'      : protocol.params.nr_of_std_low,
                                               'protocol'           : metadata['protocolName']
                                             })
                                                         
    i.aspirate_from_sample_plate_and_measure()
    
    if not protocol.is_simulating():
        i.run.export_as_csv()

def add_parameters(parameters):

    parameters.add_int(
        variable_name="nr_of_std_low",
        display_name="Number of standard low",
        description="Number of samples with buffer only",
        default = 1,
        minimum = 1,
        maximum = 4
    )

    parameters.add_int(
        variable_name="number_of_samples",
        display_name="Number of samples",
        description="The sum of <standard low> + <Number of samples> must be less or equal 96.",
        default = 1,
        minimum = 1,
        maximum = 95
    )
    
    parameters.add_bool(
        variable_name="pause_on_error",
        display_name="Pause on error",
        description="Pause on error",
        default=False
    )

    


