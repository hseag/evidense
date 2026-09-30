# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

class Quadruple:
    """Represents four wavelength-dependent values handled together."""

    def __init__(self, value230 = 0, value260 = None, value280 = None, value340 = None):
        """Initialize a quadruple.

        When only ``value230`` is provided, the value is copied to all four
        wavelengths.
        """
        if value230 != None and value260 == None and value280 == None and value340 == None:
            self.value230 = value230
            self.value260 = value230
            self.value280 = value230
            self.value340 = value230
        else:
            self.value230 = value230
            self.value260 = value260
            self.value280 = value280
            self.value340 = value340

    def __repr__(self):
        return "230={}, 260={}, 280={}, 340={}".format(self.value230, self.value260, self.value280, self.value340)

    def __add__(self, rhs):
        ret = Quadruple()

        if isinstance(rhs, Quadruple):
            ret.value230 = self.value230 + rhs.value230
            ret.value260 = self.value260 + rhs.value260
            ret.value280 = self.value280 + rhs.value280
            ret.value340 = self.value340 + rhs.value340
        else:
            ret.value230 = self.value230 + rhs
            ret.value260 = self.value260 + rhs
            ret.value280 = self.value280 + rhs
            ret.value340 = self.value340 + rhs

        return ret

    def __sub__(self, rhs):
        ret = Quadruple()

        if isinstance(rhs, Quadruple):
            ret.value230 = self.value230 - rhs.value230
            ret.value260 = self.value260 - rhs.value260
            ret.value280 = self.value280 - rhs.value280
            ret.value340 = self.value340 - rhs.value340
        else:
            ret.value230 = self.value230 - rhs
            ret.value260 = self.value260 - rhs
            ret.value280 = self.value280 - rhs
            ret.value340 = self.value340 - rhs

        return ret

    def __truediv__(self, rhs):
        ret = Quadruple()

        if isinstance(rhs, Quadruple):
            if rhs.value230 == 0:
                raise ZeroDivisionError("division by zero in Quadruple.value230")
            if rhs.value260 == 0:
                raise ZeroDivisionError("division by zero in Quadruple.value260")
            if rhs.value280 == 0:
                raise ZeroDivisionError("division by zero in Quadruple.value280")
            if rhs.value340 == 0:
                raise ZeroDivisionError("division by zero in Quadruple.value340")
            ret.value230 = self.value230 / rhs.value230
            ret.value260 = self.value260 / rhs.value260
            ret.value280 = self.value280 / rhs.value280
            ret.value340 = self.value340 / rhs.value340
        else:
            if rhs == 0:
                raise ZeroDivisionError("division by zero in Quadruple scalar division")
            ret.value230 = self.value230 / rhs
            ret.value260 = self.value260 / rhs
            ret.value280 = self.value280 / rhs
            ret.value340 = self.value340 / rhs


        return ret

    def __mul__(self, rhs):
        ret = Quadruple()

        if isinstance(rhs, Quadruple):
            ret.value230 = self.value230 * rhs.value230
            ret.value260 = self.value260 * rhs.value260
            ret.value280 = self.value280 * rhs.value280
            ret.value340 = self.value340 * rhs.value340
        else:
            ret.value230 = self.value230 * rhs
            ret.value260 = self.value260 * rhs
            ret.value280 = self.value280 * rhs
            ret.value340 = self.value340 * rhs

        return ret
        
    def __abs__(self):
        ret = Quadruple()

        ret.value230 = abs(self.value230)
        ret.value260 = abs(self.value260)
        ret.value280 = abs(self.value280)
        ret.value340 = abs(self.value340)

        return ret
        
    def __le__(self, rhs):
        return self.value230 <= rhs.value230 and self.value260 <= rhs.value260 and self.value280 <= rhs.value280 and self.value340 <= rhs.value340

    def to_json(self):
            """Convert the quadruple to a JSON-compatible dictionary."""
            return  { "value230" : self.value230, "value260" : self.value260, "value280" : self.value280, "value340" : self.value340}

    @staticmethod
    def from_json(node):
        """Create a quadruple from a JSON dictionary."""
        return Quadruple(node["value230"], node["value260"], node["value280"], node["value340"])
