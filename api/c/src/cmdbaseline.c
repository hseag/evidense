// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmdbaseline.h"
#include "printerror.h"
#include "evidense.h"
#include <stdlib.h>
#include <stdio.h>

Error_t cmdBaseline(Evi_t * self)
{
    SingleMeasurement_t measuremnt = {0};

    Error_t ret = eviDenseBaseline(self, &measuremnt);
    if (ret == ERROR_EVI_OK)
    {
        fprintf(stdout, "%i %i %i %i %i %i %i %i\n", measuremnt.channel230.sample, measuremnt.channel230.reference, measuremnt.channel260.sample, measuremnt.channel260.reference, measuremnt.channel280.sample, measuremnt.channel280.reference, measuremnt.channel340.sample, measuremnt.channel340.reference);
    }
    else
    {
        printError(ret, NULL);
    }
    return ret;
}
