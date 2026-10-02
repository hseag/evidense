# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import json

from .constants import DictKeys
from .measurement import Factors

class Kit:
    """Represents a reusable kit containing correction factors."""

    def __init__(self, factors, comment = None):
        """Initialize a kit from factors and an optional comment."""
        self.factors = factors
        self.comment = comment

    def __repr__(self):
        return "factors:{} comment:{}".format(self.factors, self.comment)

    def save(self, filename):
        """Save the kit as JSON."""
        with open(filename, 'w') as f:
            json.dump(self.to_json(), f, indent=2)
    
    @staticmethod        
    def load(filename):
        """Load a kit from a JSON file."""
        with open(filename, 'rb') as f:
            node = json.load(f)
            return Kit.from_json(node)
        return None
        
    def to_json(self):
        """Convert the kit to a JSON-compatible dictionary."""
        if self.factors is None:
            raise ValueError("Kit has no factors")
        return { DictKeys.COMMENT : self.comment, DictKeys.FACTORS : self.factors.to_json()}
     
    @staticmethod
    def from_json(node):
        """Create a kit from a JSON dictionary."""
        if node is None:
            raise ValueError("Kit JSON node is missing")
        if not isinstance(node, dict):
            raise TypeError("Kit JSON node must be a dict")

        factors_node = node.get(DictKeys.FACTORS)
        if factors_node is None:
            raise ValueError("Kit JSON does not contain factors")

        return Kit(Factors.from_json(factors_node), node.get(DictKeys.COMMENT))

    @staticmethod
    def from_run(filename, comment = None):
        """Create a kit from a JSON data file on disk."""
        with open(filename, 'rb') as f:
            node = json.load(f)
            return Kit.from_run_json(node, comment)
        return None

    @staticmethod
    def from_run_json(node, comment = None):
        """Create a kit from JSON run data."""
        if node is None:
            raise ValueError("Run JSON node is missing")
        if not isinstance(node, dict):
            raise TypeError("Run JSON node must be a dict")

        parameters = node.get(DictKeys.PARAMETERS)
        if parameters is None:
            raise ValueError("Run JSON does not contain parameters")

        kit = Kit.from_json(parameters)
        if comment is not None:
            kit.comment = comment
        return kit
        
        
