// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "channel.h"
#include "evibase.h"

Channel_t channel_init(uint32_t sample, uint32_t reference)
{
    Channel_t ret = {.sample = sample, .reference = reference};
    return ret;
}

void channel_print(const Channel_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "Sample=%i, Reference=%i%s", self->sample, self->reference, newLine ? "\n" : "");
}
