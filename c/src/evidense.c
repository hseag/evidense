// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evidense.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    SingleMeasurement_t * measurement;
} UserMeasurement;


typedef struct
{
    Levelling_t * levelling230;
    Levelling_t * levelling260;
    Levelling_t * levelling280;
    Levelling_t * levelling340;
} UserLevelling;

Error_t eviDenseMeasure_(EvieResponse_t *response, void *user)
{
    UserMeasurement *u = (UserMeasurement *)user;
    if (response->argc == 9)
    {
        u->measurement->channel230.sample    = atoi(response->argv[1]);
        u->measurement->channel230.reference = atoi(response->argv[2]);
        u->measurement->channel260.sample    = atoi(response->argv[3]);
        u->measurement->channel260.reference = atoi(response->argv[4]);
        u->measurement->channel280.sample    = atoi(response->argv[5]);
        u->measurement->channel280.reference = atoi(response->argv[6]);
        u->measurement->channel340.sample    = atoi(response->argv[7]);
        u->measurement->channel340.reference = atoi(response->argv[8]);
        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseMeasure(Evi_t * self, SingleMeasurement_t * measurement)
{
    UserMeasurement user = {.measurement = measurement};
    return eviExecute(self, "M", eviDenseMeasure_, &user);
}

Error_t eviDenseLastMeasurements(Evi_t * self, uint32_t last, SingleMeasurement_t * measurement)
{
    UserMeasurement user = {.measurement = measurement};
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
        u->measurement->channel230.sample    = atoi(response->argv[1]);
        u->measurement->channel230.reference = atoi(response->argv[2]);
        u->measurement->channel260.sample    = atoi(response->argv[3]);
        u->measurement->channel260.reference = atoi(response->argv[4]);
        u->measurement->channel280.sample    = atoi(response->argv[5]);
        u->measurement->channel280.reference = atoi(response->argv[6]);
        u->measurement->channel340.sample    = atoi(response->argv[7]);
        u->measurement->channel340.reference = atoi(response->argv[8]);
        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseBaseline(Evi_t * self, SingleMeasurement_t * measurement)
{
    UserMeasurement user = {.measurement = measurement};
    return eviExecute(self, "G", eviDenseBaseline_, &user);
}

typedef struct
{
    bool * empty;
} UserEmpty;

Error_t eviDenseIsCuvetteHolderEmpty_(EvieResponse_t *response, void *user)
{
    UserEmpty *u = (UserEmpty *)user;
    if (response->argc >= 2)
    {
        (*u->empty) = atoi(response->argv[1]);
        return ERROR_EVI_OK;
    }
    else
    {
        return ERROR_EVI_PROTOCOL_ERROR;
    }
}

Error_t eviDenseIsCuvetteHolderEmpty(Evi_t * self, bool * empty)
{
    UserEmpty user = {empty = empty};
    return eviExecute(self, "X", eviDenseIsCuvetteHolderEmpty_, &user);
}
