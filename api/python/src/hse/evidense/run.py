# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import json
import os
import logging
from enum import IntEnum

from datetime import datetime

from .constants import DictKeys
from .device import Device
from .devicesettings import DeviceSettings
from .kit import Kit
from .measurement import Factors, Measurement
from .quadruple import Quadruple
from .singlemeasurement import SingleMeasurement
from .storage import StorageMeasurement

logger = logging.getLogger(__name__)

class Run:
    """Coordinates a guided measurement run including state persistence.

    A run follows the sequence ``BASELINE -> AIR -> SAMPLE`` and stores the
    generated measurements in a JSON data file. Blank-derived factors can be
    calculated automatically and reused for subsequent sample measurements.
    """

    class State(IntEnum):                            
        BASELINE     = 0
        AIR          = 1
        SAMPLE       = 2

    @staticmethod
    def _safe_device_log_value(device):
        if device is None:
            return None
        if isinstance(device, str):
            return device
        return type(device).__name__
    
    def __init__(self, nr_of_blanks, path = None, filename = None, device = None, no_purity_ratio_260_280_correction = False, add_device_info = True):
        """Initialize a new run.

        Args:
            nr_of_blanks: Number of blank measurements at the beginning of the
                run.
            path: Optional directory used for the generated measurement file.
            filename: Optional measurement JSON filename.
            device: Device serial number, ``"SIMULATION"``, an existing
                :class:`Device` instance, or ``None`` for auto-detection.
            no_purity_ratio_260_280_correction: Disable device-settings-based
                purity ratio correction when set to ``True``.
            add_device_info: Include device metadata in the storage file.
        """
        logger.debug(
            "Run.__init__ entry: nr_of_blanks=%s path=%r filename=%r device=%r no_purity_ratio_260_280_correction=%s add_device_info=%s",
            nr_of_blanks,
            path,
            filename,
            self._safe_device_log_value(device),
            no_purity_ratio_260_280_correction,
            add_device_info,
        )
        self.nr_of_blanks  = nr_of_blanks
        self._owns_device = False
        if device is not None:
            if type(device) is str:
                self.device     = Device(device)
                self._owns_device = True
            else:
                self.device     = device
        else:
            self.device     = Device()
            self._owns_device = True
        self._count         = 0
        self._state         = self.State.BASELINE
                
        now = datetime.now()
        if filename is not None:
            self._filename  = filename
        else:
            self._filename  = "evidense-{}-{}.json".format(self.device.serial_number(), now.strftime("%Y_%m_%d_%H_%M_%S"))
            
        if path is not None:
            self._filename = os.path.join(path, self._filename)
            
        self.storage        = StorageMeasurement()
        self._factors       = None
        self._baseline      = None
        self._air           = None
        self._sample        = None

        if add_device_info:
            self.storage.add_device_info(self.device)
            self.storage.add_dict(DictKeys.PARAMETERS, self._parameters())

        self._no_purity_ratio_260_280_correction = no_purity_ratio_260_280_correction
        
        if no_purity_ratio_260_280_correction == False:
            self._device_settings = DeviceSettings.from_device(self.device)
            self.storage.add_dict(DictKeys.ADJUSTMENTS, self._device_settings.to_json())
        else:
            self._device_settings = None
        logger.debug(
            "Run.__init__ exit: filename=%r device=%r state=%s count=%s has_factors=%s add_device_info=%s",
            self._filename,
            self._safe_device_log_value(self.device),
            self._state,
            self._count,
            self._factors is not None,
            add_device_info,
        )

    def _parameters(self):
        ret = {}
        ret[DictKeys.NR_OF_BLANKS] = self.nr_of_blanks
        if self._factors:
            ret[DictKeys.FACTORS] = self._factors.to_json()
        else:
            ret[DictKeys.FACTORS] = None
        return ret
    
    def __repr__(self):
        return "device:{} nr_of_blanks:{} state:{} count:{}".format(self.device, self.nr_of_blanks, self._state, self._count)

    def close(self):
        """Release the owned device handle, if any."""
        if self._owns_device and self.device is not None:
            self.device.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc, tb):
        self.close()
        return False

    @staticmethod
    def _measurement_to_json(measurement):
        if measurement is None:
            return None
        return measurement.to_json()

    @staticmethod
    def _measurement_from_json(node):
        if node is None:
            return None
        return SingleMeasurement.from_json(node)

    @staticmethod
    def _resolve_state_filename(device = None, filename = None):
        if filename is not None:
            return filename
        if isinstance(device, str):
            return "evidense-{}-state.json".format(device)
        if device is None:
            return "state.json"
        try:
            return "evidense-{}-state.json".format(device.serial_number())
        except Exception:
            return "state.json"

    @staticmethod
    def load_state(filename = None):
        """Load a run from a previously saved state file."""
        logger.debug("Run.load_state entry: filename=%r", filename)
        filename = Run._resolve_state_filename(filename = filename)
        with open(filename, "rb") as f:
            state = json.load(f)
        measurement_filename = state.get("filename")
        run            = Run(state["nr_of_blanks"], filename = measurement_filename, device = state.get("device"), no_purity_ratio_260_280_correction = state.get("no_purity_ratio_260_280_correction"), add_device_info = False)
        run._count     = state["count"]
        run._state     = Run.State(state["state"])
        run._baseline  = Run._measurement_from_json(state.get("baseline"))
        run._air       = Run._measurement_from_json(state.get("air"))
        run._sample    = Run._measurement_from_json(state.get("sample"))
        if "factors" in state and state["factors"] is not None:
            run._factors = Factors.from_json(state["factors"])
        else:
            run._factors = None
        if measurement_filename is not None and os.path.isfile(measurement_filename):
            run.storage = StorageMeasurement(measurement_filename)
        else:
            run.storage = StorageMeasurement()

        run.re_calculate()
        logger.debug(
            "Run.load_state exit: resolved_filename=%r measurement_filename=%r state=%s count=%s has_factors=%s storage_len=%s",
            filename,
            measurement_filename,
            run._state,
            run._count,
            run._factors is not None,
            len(run.storage),
        )
        return run

    def save_state(self, filename = None):
        """Persist the current run state and measurement storage."""
        logger.debug(
            "Run.save_state entry: filename=%r state=%s count=%s measurement_filename=%r has_factors=%s",
            filename,
            self._state,
            self._count,
            self._filename,
            self._factors is not None,
        )
        filename = self._resolve_state_filename(self.device, filename)
        if self._factors is not None:
            factors = self._factors.to_json()
        else:
            factors = None
        state = {
            "filename": self._filename,
            "nr_of_blanks": self.nr_of_blanks,
            "count": self._count,
            "state": int(self._state),
            "device": "SIMULATION" if self.device.is_simulation else self.device.serial_number(),
            "baseline": self._measurement_to_json(self._baseline),
            "air": self._measurement_to_json(self._air),
            "sample": self._measurement_to_json(self._sample),
            "factors": factors,
            "no_purity_ratio_260_280_correction" : self._no_purity_ratio_260_280_correction
        }

        with open(filename, "w") as f:
            json.dump(state, f, indent=2)
            
        self.storage.save(self._filename)
        logger.debug(
            "Run.save_state exit: state_filename=%r measurement_filename=%r storage_len=%s",
            filename,
            self._filename,
            len(self.storage),
        )

    def re_calculate(self):        
        """Recalculate factors and missing results from the current storage."""
        logger.debug(
            "Run.re_calculate entry: nr_of_blanks=%s storage_len=%s has_factors=%s",
            self.nr_of_blanks,
            len(self.storage),
            self._factors is not None,
        )
        if self._factors is None and len(self.storage) >= self.nr_of_blanks and self.nr_of_blanks > 0:
            factors          = Factors()
            count            = 0.0
            one              = 1.0
            a_buffer_blank   = Quadruple(0.0)
            f                = Quadruple(0.0)
        
            for measurement in self.storage.measurements()[0:self.nr_of_blanks:]:
                factors          = factors + measurement.factors()
                count            = count + one
    
            self._factors        = factors / count
            self.storage.add_dict(DictKeys.PARAMETERS, self._parameters())
            
        if self._factors is not None:
            for entry in self.storage:
                if not entry.has_results():
                    entry.apply_results(factors = self._factors, device_settings = self._device_settings)
        logger.debug(
            "Run.re_calculate exit: has_factors=%s storage_len=%s",
            self._factors is not None,
            len(self.storage),
        )

    def measure(self, comment = None):
        """Execute the next run step.

        The method advances the internal run state machine. Every third call
        completes a baseline-air-sample sequence and appends a measurement to
        storage.
        """
        logger.debug(
            "Run.measure entry: comment=%r state=%s count=%s measurement_filename=%r",
            comment,
            self._state,
            self._count,
            self._filename,
        )
        if self._state == self.State.BASELINE:
            self._baseline = self.device.baseline()
            self._state = self.State.AIR                        
        elif self._state == self.State.AIR:
            self._air = self.device.measure()
            self._state = self.State.SAMPLE
        elif self._state == self.State.SAMPLE:
            self._sample = self.device.measure()
            measurement = Measurement(self._baseline, self._air, self._sample)
            self._state = self.State.BASELINE
            
            self.storage.append(measurement, comment, logging = self.device.logging())

        self.re_calculate()
        self.storage.save(self._filename)
        self._count = self._count + 1
        logger.debug(
            "Run.measure exit: next_state=%s count=%s storage_len=%s has_factors=%s",
            self._state,
            self._count,
            len(self.storage),
            self._factors is not None,
        )
    
    def check_empty(self) -> bool:
        """Return ``True`` if the cuvette holder is empty."""
        logger.debug("Run.check_empty entry")
        result = self.device.is_cuvette_holder_empty()
        logger.debug("Run.check_empty exit: result=%s", result)
        return result

    def export_as_csv(self):
        """Export the measurement storage as CSV."""
        logger.debug("Run.export_as_csv entry: measurement_filename=%r", self._filename)
        self.storage.export_as_csv(self._filename)
        logger.debug("Run.export_as_csv exit: measurement_filename=%r", self._filename)
        
    def export_as_kit(self, filename, comment = None):
        """Export the current factors as a kit JSON file."""
        logger.debug(
            "Run.export_as_kit entry: filename=%r comment=%r has_factors=%s",
            filename,
            comment,
            self._factors is not None,
        )
        kit = Kit(self._factors, comment)
        kit.save(filename)
        logger.debug("Run.export_as_kit exit: filename=%r", filename)
        
    def import_kit(self, filename):
        """Import factors from a kit JSON file."""
        logger.debug("Run.import_kit entry: filename=%r", filename)
        kit = Kit.load(filename)
        self.import_kit_content(kit.to_json())
        logger.debug(
            "Run.import_kit exit: filename=%r has_factors=%s measurement_filename=%r",
            filename,
            self._factors is not None,
            self._filename,
        )

    def import_kit_content(self, node):
        """Import factors from an in-memory kit JSON dictionary."""
        logger.debug("Run.import_kit_content entry")
        kit = Kit.from_json(node)
        self._factors = kit.factors
        self.storage.add_dict(DictKeys.PARAMETERS, self._parameters())
        self.storage.save(self._filename)
        logger.debug(
            "Run.import_kit_content exit: has_factors=%s measurement_filename=%r",
            self._factors is not None,
            self._filename,
        )
