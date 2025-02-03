// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmdexport.h"
#include "json.h"
#include "printerror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef enum
{
    MODE_RAW,
    MODE_MEASUREMENT
} Mode_t;

typedef struct
{
    char delimiter;
    char * filenameJson;
    char * filenameCsv;
    Mode_t mode;
} Options_t;

void exportRawMeasurement(Options_t * options, cJSON *object, FILE * csv, const char * const key, bool last)
{
    cJSON * measurement = cJSON_GetObjectItem(object, key);
    if(measurement)
    {
        cJSON * sample = NULL;
        cJSON * reference = NULL;
        cJSON *o = cJSON_GetObjectItem(object, key);
        if(o)
        {
            sample    = cJSON_GetObjectItem(o, DICT_SAMPLE);
            reference = cJSON_GetObjectItem(o, DICT_REFERENCE);
        }

        fprintf_s(csv, "%i%c", sample ? (int)cJSON_GetNumberValue(sample) : 0, options->delimiter);
        fprintf_s(csv, "%i", reference ? (int)cJSON_GetNumberValue(reference) : 0);

        if(!last)
        {
            fprintf_s(csv, "%c", options->delimiter);
        }
    }
}

void exportRawMeasurements(Options_t * options, cJSON *object, FILE * csv, const char * const key)
{
    cJSON * measurements = cJSON_GetObjectItem(object, key);

    exportRawMeasurement(options, measurements, csv, DICT_230, false);
    exportRawMeasurement(options, measurements, csv, DICT_260, false);
    exportRawMeasurement(options, measurements, csv, DICT_280, false);
    exportRawMeasurement(options, measurements, csv, DICT_340, true);
}

void exportRaw(Options_t * options, cJSON *object, FILE * csv)
{
    cJSON *iterator = NULL;
    cJSON *oComment = cJSON_GetObjectItem(object, DICT_COMMENT);

    fprintf_s(csv, "%s%c", oComment ? cJSON_GetStringValue(oComment) : "", options->delimiter);

    cJSON *oValues = cJSON_GetObjectItem(object, DICT_VALUES);
    bool first = true;

    cJSON_ArrayForEach(iterator, oValues)
    {
        if(!first)
        {
            fprintf_s(csv, "%s%c", oComment ? cJSON_GetStringValue(oComment) : "", options->delimiter);
        }

        exportRawMeasurement(options, iterator, csv, DICT_230, false);
        exportRawMeasurement(options, iterator, csv, DICT_260, false);
        exportRawMeasurement(options, iterator, csv, DICT_280, false);
        exportRawMeasurement(options, iterator, csv, DICT_340, true);

        fprintf_s(csv, "\n");
        first = false;
    }
}

void exportMeasurement(Options_t * options, cJSON *iterator, FILE * csv)
{

    cJSON *oComment = cJSON_GetObjectItem(iterator, DICT_COMMENT);

    fprintf_s(csv, "%s%c", oComment ? cJSON_GetStringValue(oComment) : "", options->delimiter);

    cJSON * oCalculated = cJSON_GetObjectItem(iterator, DICT_RESULTS);
    cJSON * dsDNA         = cJSON_GetObjectItem(oCalculated, DICT_DS_DNA);
    cJSON * ssDNA         = cJSON_GetObjectItem(oCalculated, DICT_SS_DNA);
    cJSON * ssRNA         = cJSON_GetObjectItem(oCalculated, DICT_SS_RNA);
    cJSON * purity260_230 = cJSON_GetObjectItem(oCalculated, DICT_PURITY_260_230);
    cJSON * purity260_280 = cJSON_GetObjectItem(oCalculated, DICT_PURITY_260_280);

    fprintf_s(csv, "%f%c", dsDNA ? cJSON_GetNumberValue(dsDNA) : 0.0, options->delimiter);
    fprintf_s(csv, "%f%c", ssDNA ? cJSON_GetNumberValue(ssDNA) : 0.0, options->delimiter);
    fprintf_s(csv, "%f%c", ssRNA ? cJSON_GetNumberValue(ssRNA) : 0.0, options->delimiter);
    fprintf_s(csv, "%f%c", purity260_230 ? cJSON_GetNumberValue(purity260_230) : 0.0, options->delimiter);
    fprintf_s(csv, "%f", purity260_280 ? cJSON_GetNumberValue(purity260_280) : 0.0);
    fprintf_s(csv, "\n");
}

void exportMeasurementSingleLedHeader(Options_t * options, FILE * csv, const char * const key1, const char * const key2, bool last)
{
    if(key1 == NULL)
    {
        fprintf_s(csv, "%s %s%c", key2, DICT_SAMPLE, options->delimiter);
        fprintf_s(csv, "%s %s", key2, DICT_REFERENCE);
    }
    else
    {
        fprintf_s(csv, "%s %s %s%c", key1, key2, DICT_SAMPLE, options->delimiter);
        fprintf_s(csv, "%s %s %s", key1, key2, DICT_REFERENCE);
    }

    if(!last)
    {
        fprintf_s(csv, "%c", options->delimiter);
    }
}

void exportRawHeader(Options_t * options, FILE * csv)
{
    fprintf_s(csv, "%s%c", DICT_COMMENT, options->delimiter);
    exportMeasurementSingleLedHeader(options, csv, NULL, DICT_230, false);
    exportMeasurementSingleLedHeader(options, csv, NULL, DICT_260, false);
    exportMeasurementSingleLedHeader(options, csv, NULL, DICT_280, false);
    exportMeasurementSingleLedHeader(options, csv, NULL, DICT_340, true);

    fprintf_s(csv, "\n");
}

void exportMeasurementSingleHeader(Options_t * options, FILE * csv, const char * const key)
{
   exportMeasurementSingleLedHeader(options, csv, key, DICT_230, false);
   exportMeasurementSingleLedHeader(options, csv, key, DICT_260, false);
   exportMeasurementSingleLedHeader(options, csv, key, DICT_280, false);
   exportMeasurementSingleLedHeader(options, csv, key, DICT_340, true);
}

void exportMeasurementHeader(Options_t * options, FILE * csv)
{
    fprintf_s(csv, "%s%c%s%c%s%c%s%c%s%c%s", DICT_COMMENT, options->delimiter, DICT_DS_DNA, options->delimiter, DICT_SS_DNA, options->delimiter, DICT_SS_RNA, options->delimiter, DICT_PURITY_260_230, options->delimiter, DICT_PURITY_260_280);
    fprintf_s(csv, "\n");
}

static Error_t export(Options_t * options)
{
    Error_t ret  = ERROR_EVI_OK;
    cJSON* json = jsonLoad(options->filenameJson);

    if(json)
    {
        FILE * csv = fopen(options->filenameCsv, "w");
        if(csv)
        {
            cJSON *oMeasurments = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);
            cJSON *iterator = NULL;
            switch(options->mode)
            {
                case MODE_RAW:
                    exportRawHeader(options, csv);
                    break;
                case MODE_MEASUREMENT:
                    exportMeasurementHeader(options, csv);
                    break;
            }

            cJSON_ArrayForEach(iterator, oMeasurments)
            {
                switch(options->mode)
                {
                    case MODE_RAW:
                        exportRaw(options, iterator, csv);
                        break;
                    case MODE_MEASUREMENT:
                        exportMeasurement(options, iterator, csv);
                        break;
                }

            }
            fclose(csv);
        }
        else
        {
            ret = ERROR_EVI_FILE_IO_ERROR;
        }
        cJSON_Delete(json);
    }
    else
    {
        ret = ERROR_EVI_FILE_NOT_FOUND;
    }
    return ret;
}


Error_t cmdExport(Evi_t* self, int argcCmd, char** argvCmd)
{
    Error_t ret  = ERROR_EVI_OK;
    cJSON*  json = NULL;
    Options_t options = { 0 };

    options.delimiter = ',';
    options.mode      = MODE_MEASUREMENT;

    int argcCmdSave = argcCmd;
    char **argvCmdSave = argvCmd;
    bool parsingOptions = true;
    size_t i = 1;
    
    while (i < argcCmd && parsingOptions)
    {
        if (strncmp(argvCmd[i], "--", 2) == 0 || strncmp(argvCmd[i], "-", 1) == 0)
        {
            if (strcmp(argvCmd[i], "--delimiter-comma") == 0)
            {
                options.delimiter  = ',';
            }
            else if (strcmp(argvCmd[i], "--delimiter-semicolon") == 0)
            {
                options.delimiter  = ';';
            }
            else if (strcmp(argvCmd[i], "--delimiter-tab") == 0)
            {
                options.delimiter  = '\t';
            }
            else if (strcmp(argvCmd[i], "--mode-raw") == 0)
            {
                options.mode  = MODE_RAW;
            }
            else if (strcmp(argvCmd[i], "--mode-measurement") == 0)
            {
                options.mode  = MODE_MEASUREMENT;
            }
            else
            {
                ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_OPTION, "Unknown option: %s\n", argvCmd[i]);
                goto exit;
            }
            i++;
        }
        else
        {
            parsingOptions = false;
        }
    }

    argcCmdSave = argcCmd - i;
    argvCmdSave = argvCmd + i;

    if(argcCmdSave == 2)
    {
        options.filenameJson = strdup(argvCmdSave[0]);
        options.filenameCsv  = strdup(argvCmdSave[1]);
    }
    else
    {
        ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, NULL);
        goto exit;
    }

    ret = export(&options);
exit:

    free(options.filenameJson);
    free(options.filenameCsv);

    return ret;
}
