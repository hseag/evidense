// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmddata.h"
#include "printerror.h"
#include "json.h"
#include "dict.h"
#include "measurement.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

        cJSON *json = json_loadFromFile(file);

        if (json != NULL)
        {
            cJSON *oMeasurements = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);

            if (oMeasurements)
            {
                measurement_calculate(oMeasurements, &parameters);
                json_saveToFile(file, json);
            }

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
    cJSON *json = json_loadFromFile(file);

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
