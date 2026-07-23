# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import json
import os
import csv
from datetime import datetime

from . import constants, version
from .constants import (
    DictKeys,
    Index,
)
from .measurement import Measurement, Results

class StorageMeasurementEntry:
    """Represents one entry in measurement storage."""

    def __init__(self, measurement, comment = None, results = None, node = None):
        """Initialize a storage entry."""
        self.measurement = measurement
        self.comment     = comment
        self.results     = results
        self.node        = node
        
    def __repr__(self):
        return "measurement:{} comment:{} results:{}".format(self.measurement, self.comment, self.results)
        
    def has_results(self):
        """Return ``True`` if the entry already contains calculated results."""
        if self.results is None:
            return False
        else:
            return True
            
    def apply_results(self, factors, device_settings):
        """Calculate and write results back into the underlying JSON node."""
        self.node[DictKeys.RESULTS] = self.measurement.results(factors = factors, device_settings = device_settings).to_json()
        
    @staticmethod
    def from_json(node):
        """Create a storage entry from a JSON measurement node."""
        if DictKeys.RESULTS in node:
            results = Results.from_json(node[DictKeys.RESULTS])
        else:
            results = None

        if DictKeys.COMMENT in node:
            comment = node[DictKeys.COMMENT]
        else:
            comment = None
            
        return StorageMeasurementEntry(Measurement.from_json(node), comment, results, node)

class StorageMeasurement:
    """Stores measurements and associated metadata in JSON form."""

    def __init__(self, filename = None):
        """Initialize empty storage or load it from a file."""
        
        if filename is None:
            self.data = { DictKeys.MEASUREMENTS : [] }
        else:
            with open(filename, 'rb') as f:
                self.data = json.load(f)
    
    def append(self, measurement, comment = None, logging = None):
        """Append a measurement without precomputed results."""
        if isinstance(measurement, Measurement):
            m = measurement.to_json()
        else:
            raise Exception("Class not supported!")
        
        if comment:
            m[DictKeys.COMMENT] = comment

        if logging:
            m[DictKeys.LOGGING] = logging
            
        m[DictKeys.DATE_TIME] = datetime.now().isoformat()

        self.data[DictKeys.MEASUREMENTS].append(m)

    def append_with_results(self, measurement, results, comment = None, logging = None):
        """Append a measurement together with precomputed results."""
        if isinstance(measurement, Measurement):
            m = measurement.to_json()
        else:
            raise Exception("Class not supported!")
        
        if results:
            m[DictKeys.RESULTS] = results.to_json()
            
        if comment:
            m[DictKeys.COMMENT] = comment
            
        if logging:
            m[DictKeys.LOGGING] = logging
        
        m[DictKeys.DATE_TIME] = datetime.now().isoformat()

        self.data[DictKeys.MEASUREMENTS].append(m)

    def add_device_info(self, device, comment = None):
        """Store device metadata in the JSON structure."""
        self.data[DictKeys.INFO] = { "date"                    : datetime.now().isoformat(),
                                 "product"                 : "eviDense",
                                 "production_number"       : device.production_number(),
                                 "serial_number"           : device.serial_number(),
                                 "firmware_version"        : device.firmware_version(),
                                 "comment"                 : comment or "",
                                 "api"                     : version.VERSION
                               }

    def add_dict(self, key, value):
        """Store or replace an arbitrary top-level JSON node."""
        self.data[key] = value
                               
    def save(self, filename):
        """Save the storage content as JSON."""
        with open(filename, 'w') as f:
            json.dump(self.data, f, indent=2)
            
    def measurements(self):
        """Return all stored measurements as :class:`Measurement` objects."""
        ret = []
        for m in self.data[DictKeys.MEASUREMENTS]:
            ret.append(Measurement.from_json(m))
        return ret

    def results(self):
        """Return all stored result objects."""
        ret = []
        for m in self.data[DictKeys.MEASUREMENTS]:
            ret.append(Results.from_json(m[DictKeys.RESULTS]))
        return ret
   
    def __getitem__(self, item):
        if item >= self.__len__():
            raise IndexError("CustomRange index out of range")
        return StorageMeasurementEntry.from_json(self.data[DictKeys.MEASUREMENTS][item])

    def __len__(self):
        return len(self.data[DictKeys.MEASUREMENTS])

    @staticmethod
    def export_as_csv(filenameJson):
        """Export a JSON measurement file as CSV."""
        data = {}
        if os.path.isfile(filenameJson):
            with open(filenameJson, 'rb') as f:
                data = json.load(f)
        else:
            raise Exception("File {} not found".format(filenameJson))

        filenameCsv, _ = os.path.splitext(filenameJson)
        with open('{}.csv'.format(filenameCsv), 'w', newline='') as csvfile:
            fieldnames = [constants.DictKeys.COMMENT, 
                          constants.DictKeys.BASELINE_230_REFERENCE,
                          constants.DictKeys.BASELINE_230_SAMPLE,
                          constants.DictKeys.BASELINE_260_REFERENCE,
                          constants.DictKeys.BASELINE_260_SAMPLE,
                          constants.DictKeys.BASELINE_280_REFERENCE,
                          constants.DictKeys.BASELINE_280_SAMPLE,
                          constants.DictKeys.BASELINE_340_REFERENCE,
                          constants.DictKeys.BASELINE_340_SAMPLE,
                          constants.DictKeys.AIR_230_REFERENCE,
                          constants.DictKeys.AIR_230_SAMPLE,
                          constants.DictKeys.AIR_260_REFERENCE,
                          constants.DictKeys.AIR_260_SAMPLE,
                          constants.DictKeys.AIR_280_REFERENCE,
                          constants.DictKeys.AIR_280_SAMPLE,
                          constants.DictKeys.AIR_340_REFERENCE,
                          constants.DictKeys.AIR_340_SAMPLE,
                          constants.DictKeys.SAMPLE_230_REFERENCE,
                          constants.DictKeys.SAMPLE_230_SAMPLE,
                          constants.DictKeys.SAMPLE_260_REFERENCE,
                          constants.DictKeys.SAMPLE_260_SAMPLE,
                          constants.DictKeys.SAMPLE_280_REFERENCE,
                          constants.DictKeys.SAMPLE_280_SAMPLE,
                          constants.DictKeys.SAMPLE_340_REFERENCE,
                          constants.DictKeys.SAMPLE_340_SAMPLE,
                          constants.DictKeys.DSDNA,
                          constants.DictKeys.SSDNA,
                          constants.DictKeys.SSRNA,
                          constants.DictKeys.A230,
                          constants.DictKeys.A260,
                          constants.DictKeys.A280,
                          constants.DictKeys.A340,
                          constants.DictKeys.PURITY260_230,
                          constants.DictKeys.PURITY260_280,
                          ]

            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter=";")
            writer.writeheader()
            for measurement in data[constants.DictKeys.MEASUREMENTS]:
                dsDNA = ""
                ssDNA = ""
                ssRNA = ""
                a230 = ""
                a260 = ""
                a280 = ""
                a340 = ""
                purity260_230 = ""
                purity260_280 = ""
                
                if measurement.get(constants.DictKeys.RESULTS) != None:
                        results        = measurement[constants.DictKeys.RESULTS]
                        dsDNA         = results[constants.DictKeys.DSDNA]
                        ssDNA         = results[constants.DictKeys.SSDNA]
                        ssRNA         = results[constants.DictKeys.SSRNA]
                        a230          = results.get(constants.DictKeys.A230, "")
                        a260          = results.get(constants.DictKeys.A260, "")
                        a280          = results.get(constants.DictKeys.A280, "")
                        a340          = results.get(constants.DictKeys.A340, "")
                        purity260_230 = results[constants.DictKeys.PURITY260_230]
                        purity260_280 = results[constants.DictKeys.PURITY260_280]
                        
                writer.writerow({constants.DictKeys.COMMENT:          measurement.get(constants.DictKeys.COMMENT, ""),
                                 constants.DictKeys.BASELINE_230_REFERENCE:     measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_230][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.BASELINE_230_SAMPLE:        measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_230][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.BASELINE_260_REFERENCE:     measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_260][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.BASELINE_260_SAMPLE:        measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_260][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.BASELINE_280_REFERENCE:     measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_280][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.BASELINE_280_SAMPLE:        measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_280][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.BASELINE_340_REFERENCE:     measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_340][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.BASELINE_340_SAMPLE:        measurement[constants.DictKeys.BASELINE][constants.DictKeys.CH_340][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.AIR_230_REFERENCE:          measurement[constants.DictKeys.AIR][constants.DictKeys.CH_230][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.AIR_230_SAMPLE:             measurement[constants.DictKeys.AIR][constants.DictKeys.CH_230][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.AIR_260_REFERENCE:          measurement[constants.DictKeys.AIR][constants.DictKeys.CH_260][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.AIR_260_SAMPLE:             measurement[constants.DictKeys.AIR][constants.DictKeys.CH_260][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.AIR_280_REFERENCE:          measurement[constants.DictKeys.AIR][constants.DictKeys.CH_280][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.AIR_280_SAMPLE:             measurement[constants.DictKeys.AIR][constants.DictKeys.CH_280][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.AIR_340_REFERENCE:          measurement[constants.DictKeys.AIR][constants.DictKeys.CH_340][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.AIR_340_SAMPLE:             measurement[constants.DictKeys.AIR][constants.DictKeys.CH_340][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.SAMPLE_230_REFERENCE:       measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_230][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.SAMPLE_230_SAMPLE:          measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_230][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.SAMPLE_260_REFERENCE:       measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_260][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.SAMPLE_260_SAMPLE:          measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_260][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.SAMPLE_280_REFERENCE:       measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_280][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.SAMPLE_280_SAMPLE:          measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_280][constants.DictKeys.SAMPLE],
                                 constants.DictKeys.SAMPLE_340_REFERENCE:       measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_340][constants.DictKeys.REFERENCE],
                                 constants.DictKeys.SAMPLE_340_SAMPLE:          measurement[constants.DictKeys.SAMPLE][constants.DictKeys.CH_340][constants.DictKeys.SAMPLE],

                                 constants.DictKeys.DSDNA:            dsDNA,
                                 constants.DictKeys.SSDNA:            ssDNA,
                                 constants.DictKeys.SSRNA:            ssRNA,
                                 constants.DictKeys.A230:             a230,
                                 constants.DictKeys.A260:             a260,
                                 constants.DictKeys.A280:             a280,
                                 constants.DictKeys.A340:             a340,
                                 constants.DictKeys.PURITY260_230:    purity260_230,
                                 constants.DictKeys.PURITY260_280:    purity260_280})
        
