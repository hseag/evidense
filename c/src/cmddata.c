// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmddata.h"
#include "printerror.h"
#include "json.h"
#include "measurement.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint32_t blanksStart;
    uint32_t blanksEnd;
    double   cuvettePathLength;

} Parameters_t;

Parameters_t parametersCreate()
{
    Parameters_t ret;

    ret.blanksStart       = 1;
    ret.blanksEnd         = 0;
    ret.cuvettePathLength = 1.1; //mm

    return ret;
}

typedef struct
{
    Quadruple_t fAbsorbanceBufferBlank;
} Factors_t;

static Channel_t channel_fromJson(cJSON * node, const char * key)
{
    Channel_t ret = {0};

    cJSON *oChannel = cJSON_GetObjectItem(node, key);

    if(oChannel)
    {
        cJSON *oSample = cJSON_GetObjectItem(oChannel, DICT_SAMPLE);
        if(oSample)
        {
            ret.sample = cJSON_GetNumberValue(oSample);
        }

        cJSON *oReference = cJSON_GetObjectItem(oChannel, DICT_REFERENCE);
        if(oReference)
        {
            ret.reference = cJSON_GetNumberValue(oReference);
        }
    }

    return ret;
}

static SingleMeasurement_t singleMeasurement_fromJson(cJSON * node, const char * key)
{
    cJSON *o = cJSON_GetObjectItem(node, key);

    Channel_t channel230 = channel_fromJson(o, DICT_230);
    Channel_t channel260 = channel_fromJson(o, DICT_260);
    Channel_t channel280 = channel_fromJson(o, DICT_280);
    Channel_t channel340 = channel_fromJson(o, DICT_340);

    SingleMeasurement_t s = singleMeasurement_init(channel230, channel260, channel280, channel340);
    return s;
}

static Measurement_t measurement_fromJson(cJSON * node)
{
    SingleMeasurement_t baseline = singleMeasurement_fromJson(node, DICT_BASELINE);
    SingleMeasurement_t air      = singleMeasurement_fromJson(node, DICT_AIR);
    SingleMeasurement_t sample   = singleMeasurement_fromJson(node, DICT_SAMPLE);

    Measurement_t m = measurement_init(baseline, air, sample, NULL);
    return m;
}

static Factors_t calculateFactors(cJSON *oMeasurments, const Parameters_t * parameters)
{
    Factors_t factors;
    cJSON *iterator = NULL;
    uint32_t index = 0;
    uint32_t count = 0;

    uint32_t length = cJSON_GetArraySize(oMeasurments);

    factors.fAbsorbanceBufferBlank = quadruple_initAllTheSame(0.0);

    cJSON_ArrayForEach(iterator, oMeasurments)
    {
        if ((index < parameters->blanksStart) || (index >= (length - parameters->blanksEnd)))
        {
            Measurement_t m = measurement_fromJson(iterator);
            Quadruple_t f1  = measurement_factorAbsorbanceBufferBlank(&m);
            factors.fAbsorbanceBufferBlank = quadruple_add(&factors.fAbsorbanceBufferBlank, &f1);
            count++;
        }
        index++;
    }

    if (count == 0)
    {
        factors.fAbsorbanceBufferBlank = quadruple_initAllTheSame(1.0);
    }
    else
    {
        Quadruple_t qCount       = quadruple_initAllTheSame(count);

        factors.fAbsorbanceBufferBlank = quadruple_div(&factors.fAbsorbanceBufferBlank, &qCount);
    }
    return factors;
}

static cJSON *calculate(cJSON *measurement, const Factors_t * factors, const Parameters_t * parameters)
{
    cJSON *obj = cJSON_CreateObject();

    Measurement_t m = measurement_fromJson(measurement);

    cJSON_AddNumberToObject(obj, DICT_DS_DNA, measurement_dsDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));
    cJSON_AddNumberToObject(obj, DICT_SS_DNA, measurement_ssDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));
    cJSON_AddNumberToObject(obj, DICT_SS_RNA, measurement_ssRNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));

    cJSON_AddNumberToObject(obj, DICT_PURITY_260_230, measurement_purityRatio260_230(&m, &factors->fAbsorbanceBufferBlank));
    cJSON_AddNumberToObject(obj, DICT_PURITY_260_280, measurement_purityRatio260_280(&m, &factors->fAbsorbanceBufferBlank));

    return obj;
}

static Error_t cmdCalculate(Evi_t *self, int argcCmd, char **argvCmd)
{
    Error_t ret = ERROR_EVI_OK;
    Parameters_t parameters = parametersCreate();
    bool options = true;
    int i = 0;

    while (i < argcCmd && options)
    {
        if (strncmp(argvCmd[i], "--", 2) == 0 || strncmp(argvCmd[i], "-", 1) == 0)
        {
            if ((strcmp(argvCmd[i], "--blanksStart") == 0) && (i + 1 < argcCmd))
            {
                i++;
                parameters.blanksStart = atoi(argvCmd[i]);
            }
            else if ((strcmp(argvCmd[i], "--blanksEnd") == 0) && (i + 1 < argcCmd))
            {
                i++;
                parameters.blanksEnd = atoi(argvCmd[i]);
            }
            else if ((strcmp(argvCmd[i], "--pathLength") == 0) && (i + 1 < argcCmd))
            {
                i++;
                parameters.cuvettePathLength = atof(argvCmd[i]);
            }
            else
            {
                return printError(ERROR_EVI_UNKOWN_COMMAND_LINE_OPTION, "Unknown option: %s\n", argvCmd[i]);
            }
            i++;
        }
        else
        {
            options = false;
        }
    }

    if (i < argcCmd)
    {
        char *file = argvCmd[i];

        cJSON *json = jsonLoad(file);

        if (json != NULL)
        {
            cJSON *oMeasurments = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);

            if (oMeasurments)
            {
                Factors_t factors = calculateFactors(oMeasurments, &parameters);
                cJSON *iterator = NULL;
                cJSON_ArrayForEach(iterator, oMeasurments)
                {
                    cJSON_DeleteItemFromObject(iterator, DICT_RESULTS);
                    cJSON_AddItemToObject(iterator, DICT_RESULTS, calculate(iterator, &factors, &parameters));
                }
            }

            jsonSave(file, json);

            cJSON_Delete(json);
        }
        else
        {
            ret = ERROR_EVI_FILE_NOT_FOUND;
            printError(ret, "File %s not found.", file);
        }
    }
    return ret;
}

static Error_t cmdDataPrint(Evi_t *self, char *file)
{
    cJSON *json = jsonLoad(file);

    if (json != NULL)
    {
        cJSON *oMeasurments = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);
        if (oMeasurments)
        {
            cJSON *iterator = NULL;
            cJSON_ArrayForEach(iterator, oMeasurments)
            {
                cJSON *oCalculated = cJSON_GetObjectItem(iterator, DICT_RESULTS);
                cJSON *oComment = cJSON_GetObjectItem(iterator, DICT_COMMENT);
                if (oCalculated)
                {
                    cJSON *oDsDNA         = cJSON_GetObjectItem(oCalculated, DICT_DS_DNA);
                    cJSON *oSsDNA         = cJSON_GetObjectItem(oCalculated, DICT_SS_DNA);
                    cJSON *oSsRNA         = cJSON_GetObjectItem(oCalculated, DICT_SS_RNA);
                    cJSON *oPurity260_230 = cJSON_GetObjectItem(oCalculated, DICT_PURITY_260_230);
                    cJSON *oPurity260_280 = cJSON_GetObjectItem(oCalculated, DICT_PURITY_260_280);


                    fprintf_s(stdout, "%f %f %f  %f %f ",
                            oDsDNA         ? cJSON_GetNumberValue(oDsDNA)         : 0.0,
                            oSsDNA         ? cJSON_GetNumberValue(oSsDNA)         : 0.0,
                            oSsRNA         ? cJSON_GetNumberValue(oSsRNA)         : 0.0,
                            oPurity260_230 ? cJSON_GetNumberValue(oPurity260_230) : 0.0,
                            oPurity260_280 ? cJSON_GetNumberValue(oPurity260_280) : 0.0);
                    if (oComment)
                    {
                        fprintf_s(stdout, "%s ", cJSON_GetStringValue(oComment));
                    }
                    fprintf_s(stdout, "\n");
                }
            }
        }
        cJSON_Delete(json);
        return ERROR_EVI_OK;
    }
    else
    {
        printError(ERROR_EVI_FILE_NOT_FOUND, "File %s not found.", file);
        return ERROR_EVI_FILE_NOT_FOUND;
    }
}

Error_t cmdData(Evi_t *self, int argcCmd, char **argvCmd)
{
    Error_t ret = ERROR_EVI_OK;

    if ((argcCmd >= 3) && (strcmp(argvCmd[1], "calculate") == 0))
    {
        ret = cmdCalculate(self, argcCmd - 2, argvCmd + 2);
    }
    else if ((argcCmd == 3) && (strcmp(argvCmd[1], "print") == 0))
    {
        ret = cmdDataPrint(self, argvCmd[2]);
    }
    else
    {
        ret = ERROR_EVI_INVALID_PARAMETER;
    }

    if (ret != ERROR_EVI_OK)
    {
        printError(ret, NULL);
    }

    return ret;
}
