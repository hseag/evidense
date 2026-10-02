# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

from .constants import DictKeys

class Channel:
    """Represents one wavelength channel with sample and reference values."""

    def __init__(self, sample = 0.0, reference = 0.0):
        """Initialize a channel.

        Args:
            sample: Sample value in uV.
            reference: Reference value in uV.
        """
        self.sample    = sample
        self.reference = reference
        
    def __repr__(self):
        return "Sample:{} Reference:{}".format(self.sample, self.reference)

    def to_json(self):
        """Convert the channel to a JSON-compatible dictionary."""
        return { DictKeys.SAMPLE : self.sample, DictKeys.REFERENCE : self.reference}
     
    @staticmethod
    def from_json(node):
        """Create a channel from a JSON dictionary."""
        return Channel(node[DictKeys.SAMPLE], node[DictKeys.REFERENCE])
