// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evibase.h"
#include "dict.h"
#include "singlemeasurement.h"

SingleMeasurement_t singleMeasurement_init(Channel_t channel230, Channel_t channel260, Channel_t channel280, Channel_t channel340)
{
    SingleMeasurement_t ret = {.channel230 = channel230,
                               .channel260 = channel260,
                               .channel280 = channel280,
                               .channel340 = channel340};

    return ret;
}

Quadruple_t singleMeasurement_sample(const SingleMeasurement_t * self)
{
    Quadruple_t ret = {.value230 = self->channel230.sample,
                       .value260 = self->channel260.sample,
                       .value280 = self->channel280.sample,
                       .value340 = self->channel340.sample};
    return ret;
}

Quadruple_t singleMeasurement_reference(const SingleMeasurement_t * self)
{
    Quadruple_t ret = {.value230 = self->channel230.reference,
                       .value260 = self->channel260.reference,
                       .value280 = self->channel280.reference,
                       .value340 = self->channel340.reference};
    return ret;
}

void singleMeasurement_print(const SingleMeasurement_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "230: [");
    channel_print(&self->channel230, stream, false);
    fprintf_s(stream, "]\n");

    fprintf_s(stream, "260: [");
    channel_print(&self->channel260, stream, false);
    fprintf_s(stream, "]\n");

    fprintf_s(stream, "280: [");
    channel_print(&self->channel280, stream, false);
    fprintf_s(stream, "]\n");

    fprintf_s(stream, "340: [");
    channel_print(&self->channel340, stream, false);
    fprintf_s(stream, "]%s", newLine ? "\n" : "");
}

cJSON* singleMeasurement_toJson(const SingleMeasurement_t * measurement)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, DICT_230, channel_toJson(&measurement->channel230));
    cJSON_AddItemToObject(obj, DICT_260, channel_toJson(&measurement->channel260));
    cJSON_AddItemToObject(obj, DICT_280, channel_toJson(&measurement->channel280));
    cJSON_AddItemToObject(obj, DICT_340, channel_toJson(&measurement->channel340));
    return obj;
}

bool singleMeasurement_fromJson(cJSON* obj, SingleMeasurement_t * measurement)
{
    bool ret = false;

    cJSON * o230 = cJSON_GetObjectItem(obj, DICT_230);
    cJSON * o260 = cJSON_GetObjectItem(obj, DICT_260);
    cJSON * o280 = cJSON_GetObjectItem(obj, DICT_280);
    cJSON * o340 = cJSON_GetObjectItem(obj, DICT_340);
    if(o230 && o260 && o280 && o340)
    {
        ret = channel_fromJson(o230, &measurement->channel230) &&
              channel_fromJson(o260, &measurement->channel260) &&
              channel_fromJson(o280, &measurement->channel280) &&
              channel_fromJson(o340, &measurement->channel340);
    }
    return ret;
}
