// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evidense.h"
#include <stdio.h>

Error_t eviDenseMeasure_(EvieResponse_t *response, void *user)
{
    UserMeasurement *u = (UserMeasurement *)user;
    if (response->argc == 9)
    {
        *(u->sample230)    = atoi(response->argv[1]);
        *(u->reference230) = atoi(response->argv[2]);
        *(u->sample260)    = atoi(response->argv[3]);
        *(u->reference260) = atoi(response->argv[4]);
        *(u->sample280)    = atoi(response->argv[5]);
        *(u->reference280) = atoi(response->argv[6]);
        *(u->sample340)    = atoi(response->argv[7]);
        *(u->reference340) = atoi(response->argv[8]);
        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseMeasure(Evi_t * self, uint32_t * sample230, uint32_t * reference230, uint32_t * sample260, uint32_t * reference260, uint32_t * sample280, uint32_t * reference280, uint32_t * sample340, uint32_t * reference340)
{
    UserMeasurement user = {sample230 = sample230, reference230 = reference230, sample260 = sample260, reference260 = reference260, sample280 = sample280, reference280 = reference280, sample340 = sample340, reference340 = reference340};
    return eviExecute(self, "M", eviDenseMeasure_, &user);
}

Error_t eviDenseLastMeasurements(Evi_t * self, uint32_t last, uint32_t * sample230, uint32_t * reference230, uint32_t * sample260, uint32_t * reference260, uint32_t * sample280, uint32_t * reference280, uint32_t * sample340, uint32_t * reference340)
{
    UserMeasurement user = {sample230 = sample230, reference230 = reference230, sample260 = sample260, reference260 = reference260, sample280 = sample280, reference280 = reference280, sample340 = sample340, reference340 = reference340};
    char cmd[EVI_MAX_LINE_LENGTH];
    sprintf_s(cmd, sizeof(cmd), "M %i", last);
    return eviExecute(self, cmd, eviDenseMeasure_, &user);
}

Error_t eviDenseLevelling_(EvieResponse_t *response, void *user)
{
    UserLevelling *u = (UserLevelling *)user;
    if (response->argc == 17)
    {
        u->levelling230->result                 = atoi(response->argv[1]);
        u->levelling230->current                = atoi(response->argv[2]);
        u->levelling230->amplificationSample    = atoi(response->argv[3]);
        u->levelling230->amplificationReference = atoi(response->argv[4]);

        u->levelling260->result                 = atoi(response->argv[5]);
        u->levelling260->current                = atoi(response->argv[6]);
        u->levelling260->amplificationSample    = atoi(response->argv[7]);
        u->levelling260->amplificationReference = atoi(response->argv[8]);

        u->levelling280->result                 = atoi(response->argv[9]);
        u->levelling280->current                = atoi(response->argv[10]);
        u->levelling280->amplificationSample    = atoi(response->argv[11]);
        u->levelling280->amplificationReference = atoi(response->argv[12]);

        u->levelling340->result                 = atoi(response->argv[13]);
        u->levelling340->current                = atoi(response->argv[14]);
        u->levelling340->amplificationSample    = atoi(response->argv[15]);
        u->levelling340->amplificationReference = atoi(response->argv[16]);

        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseLevelling(Evi_t * self, Levelling_t * levelling230, Levelling_t * levelling260, Levelling_t * levelling280, Levelling_t * levelling340)
{
    UserLevelling user = {levelling230 = levelling230, levelling260 = levelling260, levelling280 = levelling280, levelling340 = levelling340};
    return eviExecute(self, "C", eviDenseLevelling_, &user);
}

Error_t eviDenseLastLevelling(Evi_t * self, Levelling_t * levelling230, Levelling_t * levelling260, Levelling_t * levelling280, Levelling_t * levelling340)
{
    UserLevelling user = {levelling230 = levelling230, levelling260 = levelling260, levelling280 = levelling280, levelling340 = levelling340};
    return eviExecute(self, "C 0", eviDenseLevelling_, &user);
}

Error_t eviDenseBaseline_(EvieResponse_t *response, void *user)
{
    UserMeasurement *u = (UserMeasurement *)user;
    if (response->argc == 9)
    {
        *(u->sample230)    = atoi(response->argv[1]);
        *(u->reference230) = atoi(response->argv[2]);
        *(u->sample260)    = atoi(response->argv[3]);
        *(u->reference260) = atoi(response->argv[4]);
        *(u->sample280)    = atoi(response->argv[5]);
        *(u->reference280) = atoi(response->argv[6]);
        *(u->sample340)    = atoi(response->argv[7]);
        *(u->reference340) = atoi(response->argv[8]);
        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseBaseline(Evi_t * self, uint32_t * sample230, uint32_t * reference230, uint32_t * sample260, uint32_t * reference260, uint32_t * sample280, uint32_t * reference280, uint32_t * sample340, uint32_t * reference340)
{
    UserMeasurement user = {sample230 = sample230, reference230 = reference230, sample260 = sample260, reference260 = reference260, sample280 = sample280, reference280 = reference280, sample340 = sample340, reference340 = reference340};
    return eviExecute(self, "G", eviDenseBaseline_, &user);
}
