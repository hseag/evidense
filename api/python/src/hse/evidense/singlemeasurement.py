# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

from .channel import Channel
from .constants import DictKeys
from .quadruple import Quadruple

class SingleMeasurement:
    """Represents one raw device measurement across all four wavelengths."""

    def __init__(self, channel230 = None, channel260 = None, channel280 = None, channel340 = None):
        """Initialize a single measurement from its wavelength channels."""
        self.channel230 = Channel() if channel230 is None else channel230
        self.channel260 = Channel() if channel260 is None else channel260
        self.channel280 = Channel() if channel280 is None else channel280
        self.channel340 = Channel() if channel340 is None else channel340
        
    def __repr__(self):
        return "230: [{}] 260: [{}] 280: [{}] 340: [{}]".format(self.channel230, self.channel260, self.channel280, self.channel340)
        
    def to_json(self):
        """Convert the measurement to a JSON-compatible dictionary."""
        return { DictKeys.CH_230 :  self.channel230.to_json(),
                 DictKeys.CH_260 :  self.channel260.to_json(),
                 DictKeys.CH_280 :  self.channel280.to_json(), 
                 DictKeys.CH_340 :  self.channel340.to_json()
               }
               
    def sample(self):
        """Return the sample values as a :class:`Quadruple`."""
        return Quadruple(self.channel230.sample, self.channel260.sample, self.channel280.sample, self.channel340.sample)

    def reference(self):
        """Return the reference values as a :class:`Quadruple`."""
        return Quadruple(self.channel230.reference, self.channel260.reference, self.channel280.reference, self.channel340.reference)
               
    @staticmethod
    def from_json(node):
        """Create a measurement from a JSON dictionary."""
        return SingleMeasurement(Channel.from_json(node[DictKeys.CH_230]), Channel.from_json(node[DictKeys.CH_260]), Channel.from_json(node[DictKeys.CH_280]), Channel.from_json(node[DictKeys.CH_340]))
