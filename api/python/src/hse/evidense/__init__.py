# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

from . import constants, version
from . import service
from .channel import Channel
from .constants import DictKeys, Error, Index, Selftest, TypeOf
from .device import Device, LevellingChannelResult, LevellingResult, SelfttestResult
from .devicesettings import DeviceSettings
from .kit import Kit
from .measurement import DEFAULT_CUVETTE_PATH_LENGTH, Factors, Measurement, Results
from .quadruple import Quadruple
from .run import Run
from .singlemeasurement import SingleMeasurement
from .storage import StorageMeasurement, StorageMeasurementEntry
from .version import VERSION

_constant_exports = {name: getattr(DictKeys, name) for name in dir(DictKeys) if name.isupper()}
globals().update(_constant_exports)

__all__ = [
    "VERSION",
    "Quadruple",
    "Channel",
    "SingleMeasurement",
    "Results",
    "Factors",
    "Measurement",
    "DEFAULT_CUVETTE_PATH_LENGTH",
    "Kit",
    "DeviceSettings",
    "LevellingChannelResult",
    "LevellingResult",
    "SelfttestResult",
    "Device",
    "StorageMeasurementEntry",
    "StorageMeasurement",
    "Run",
    "Error",
    "Index",
    "Selftest",
    "TypeOf",
    "DictKeys",
    "constants",
    "service",
    "version",
    *_constant_exports,
]
