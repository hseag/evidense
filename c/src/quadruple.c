// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "quadruple.h"
#include "evibase.h"

Quadruple_t quadruple_initAllTheSame(double value)
{
    return quadruple_init(value, value, value, value);
}

Quadruple_t quadruple_init(double value230, double value260, double value280, double value340)
{
    Quadruple_t ret = {.value230 = value230,
                       .value260 = value260,
                       .value280 = value280,
                    .   value340 = value340,};
    return ret;
}

Quadruple_t quadruple_add(const Quadruple_t * self, const Quadruple_t * rhs)
{
    Quadruple_t ret;

    ret.value230 = self->value230 + rhs->value230;
    ret.value260 = self->value260 + rhs->value260;
    ret.value280 = self->value280 + rhs->value280;
    ret.value340 = self->value340 + rhs->value340;

    return ret;
}

Quadruple_t quadruple_sub(const Quadruple_t * self, const Quadruple_t * rhs)
{
    Quadruple_t ret;

    ret.value230 = self->value230 - rhs->value230;
    ret.value260 = self->value260 - rhs->value260;
    ret.value280 = self->value280 - rhs->value280;
    ret.value340 = self->value340 - rhs->value340;

    return ret;
}

Quadruple_t quadruple_div(const Quadruple_t * self, const Quadruple_t * rhs)
{
    Quadruple_t ret;

    ret.value230 = self->value230 / rhs->value230;
    ret.value260 = self->value260 / rhs->value260;
    ret.value280 = self->value280 / rhs->value280;
    ret.value340 = self->value340 / rhs->value340;

    return ret;
}

Quadruple_t quadruple_mul(const Quadruple_t * self, const Quadruple_t * rhs)
{
    Quadruple_t ret;

    ret.value230 = self->value230 * rhs->value230;
    ret.value260 = self->value260 * rhs->value260;
    ret.value280 = self->value280 * rhs->value280;
    ret.value340 = self->value340 * rhs->value340;

    return ret;
}

void quadruple_print(const Quadruple_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "230=%f, 260=%f, 280=%f, 340=%f%s", self->value230, self->value260, self->value280, self->value340, newLine ? "\n" : "");
}
