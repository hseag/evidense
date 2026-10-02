# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

from .constants import DictKeys, Index

class DeviceSettings:
    """Device-specific wavelength settings used for purity correction."""

    def __init__(self, center_wavelength_260 = 260.0, center_wavelength_280 = 280.0):
        """Initialize device settings.

        Args:
            center_wavelength_260: Effective center wavelength of the 260 nm
                channel in nm.
            center_wavelength_280: Effective center wavelength of the 280 nm
                channel in nm.
        """
        self.center_wavelength_260 = center_wavelength_260
        self.center_wavelength_280 = center_wavelength_280

    def __repr__(self):
        return "Center wavelength 260:{} Center wavelength 280:{}".format(self.center_wavelength_260, self.center_wavelength_280)

    def to_json(self):
        """Convert the settings to a JSON-compatible dictionary."""
        ret = {}
        ret[DictKeys.CENTER_WAVELENGTHS] = { DictKeys.CH_260 : self.center_wavelength_260, DictKeys.CH_280 : self.center_wavelength_280}
        return ret 

    @staticmethod
    def from_device(device):
        """Read device-specific wavelength settings from a device."""
        device_settings = DeviceSettings()
        # The eviDense device stores the wavelengths in pm, but the SW API excepts nm.
        try:
            device_settings.center_wavelength_260 = device.get(Index.LED260NM_CENTER_WAVE_LENGTH) / 1000.0
        except:
            pass
        
        try:
            device_settings.center_wavelength_280 = device.get(Index.LED280NM_CENTER_WAVE_LENGTH) / 1000.0
        except:
            pass
        return device_settings

    @staticmethod
    def from_json(node):
        """Create device settings from a JSON dictionary."""
        if DictKeys.CENTER_WAVELENGTHS not in node:
            return DeviceSettings()
        else:
          center_wavelength_260 = 260.0
          center_wavelength_280 = 280.0
          if DictKeys.CH_260 in node[DictKeys.CENTER_WAVELENGTHS]:
              center_wavelength_260 = node[DictKeys.CENTER_WAVELENGTHS][DictKeys.CH_260]
          if DictKeys.CH_280 in node[DictKeys.CENTER_WAVELENGTHS]:
              center_wavelength_280 = node[DictKeys.CENTER_WAVELENGTHS][DictKeys.CH_280]

          return DeviceSettings(center_wavelength_260 = center_wavelength_260, center_wavelength_280 = center_wavelength_280)
