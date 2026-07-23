// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evibase.h"
#include "dict.h"
#include "channel.h"

Channel_t channel_init(uint32_t sample, uint32_t reference)
{
    Channel_t ret = {.sample = sample, .reference = reference};
    return ret;
}

void channel_print(const Channel_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "Sample=%i, Reference=%i%s", self->sample, self->reference, newLine ? "\n" : "");
}

cJSON* channel_toJson(const Channel_t * channel)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, DICT_SAMPLE, cJSON_CreateNumber(channel->sample));
    cJSON_AddItemToObject(obj, DICT_REFERENCE, cJSON_CreateNumber(channel->reference));
    return obj;
}

DLLEXPORT bool channel_fromJson(cJSON* obj, Channel_t * channel)
{
    bool ret = false;

    if(obj)
    {
        cJSON * oSample = cJSON_GetObjectItem(obj, DICT_SAMPLE);
        cJSON *oReference = cJSON_GetObjectItem(obj, DICT_REFERENCE);

        if(oSample && oReference)
        {
            channel->sample = cJSON_GetNumberValue(oSample);
            channel->reference = cJSON_GetNumberValue(oReference);
            ret = true;
        }
    }

    return ret;
}
