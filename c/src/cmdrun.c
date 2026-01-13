// DX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmdrun.h"
#include "cmdsave.h"
#include "commonindex.h"
#include "measurement.h"
#include "cmdexport.h"
#include "cJSON.h"
#include "json.h"
#include "dict.h"
#include "printerror.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

#if defined(_WIN64) || defined(_WIN32)
#else
#include <unistd.h>
#endif

typedef struct
{
    char * filename_state;
	char * filename_data;
} Options_t;

typedef enum
{
    StateBaseline    = 0,
    StateAir         = 1,
    StateSample      = 2,
} State_t;

#define DICT_CONTEXT_NROFBLANKS           "nrOfBlanks"
#define DICT_CONTEXT_STATE                "state"
#define DICT_CONTEXT_DATA_FILE            "dataFile"
#define DICT_CONTEXT_COUNT                "count"
#define DICT_CONTEXT_LOG                  "log"
#define DICT_CONTEXT_LOG_TIME             "time"
#define DICT_CONTEXT_LOG_TEXT             "text"

#define DICT_CONTEXT_DATA                 "data"

#define DICT_CONTEXT_DATA_BASELINE        "baseline"
#define DICT_CONTEXT_DATA_AIR             "air"


static void loggingClear(Evi_t * self)
{
    char line[EVI_MAX_LINE_LENGTH];
    Error_t ret  = ERROR_EVI_OK;
    while(ret == ERROR_EVI_OK)
    {
        ret = eviLogging(self, line, sizeof(line));
    }
}

static cJSON * contextCreate(cJSON * context)
{
    cJSON_Delete(context);
    cJSON * json = cJSON_CreateObject();
    return json;
}

static cJSON * contextLoad(char * filename)
{
    cJSON* json = json_loadFromFile(filename);
    if(json)
    {
        //do nothing
    }
    else
    {
        json  = cJSON_CreateObject();
    }
    return json;
}

static void contextSave(cJSON * context, char * filename)
{
    FILE* fout   = 0;
    char* buffer = NULL;

    fout = fopen(filename, "w");

    buffer = cJSON_Print(context);
    if(buffer != NULL)
    {
        fwrite(buffer, strlen(buffer), 1, fout);
        free(buffer);
    }

    fclose(fout);
}

static void contextAddLog(cJSON * context, const char * text, ...)
{
    char * ts  = malloc_timeStamp(TimeStampTypeISO8601);

    va_list args;
    va_start(args, text);

    char * msg = malloc_vprintf(text, args);
    va_end(args);

    cJSON * cTime = cJSON_CreateString(ts);
    cJSON * cText = cJSON_CreateString(msg);
    cJSON * item  = cJSON_CreateObject();

    cJSON_AddItemToObject(item, DICT_CONTEXT_LOG_TIME, cTime);
    cJSON_AddItemToObject(item, DICT_CONTEXT_LOG_TEXT, cText);

    cJSON * log = cJSON_GetObjectItem(context, DICT_CONTEXT_LOG);
    if(log == NULL)
    {
        log = cJSON_CreateArray();
        cJSON_AddItemToObject(context, DICT_CONTEXT_LOG, log);
    }
    cJSON_AddItemToArray(log, item);
    free(msg);
    free(ts);
}

static void contextSetNumber(cJSON * context, const char * string, double number)
{
    cJSON * o = cJSON_GetObjectItem(context, string);
    if(o == NULL)
    {
        cJSON_AddItemToObject(context, string, cJSON_CreateNumber(number));
    }
    else
    {
        cJSON_SetNumberValue(o, number);
    }
}

static double contextGetNumber(cJSON * context, const char * string)
{
    cJSON * o = cJSON_GetObjectItem(context, string);
    if(o == NULL)
    {
        return 0.0;
    }
    else
    {
        return cJSON_GetNumberValue(o);
    }
}

static void contextSetString(cJSON * context, const char * string, const char * valueString)
{
    cJSON * o = cJSON_GetObjectItem(context, string);
    if(o == NULL)
    {
        cJSON_AddItemToObject(context, string, cJSON_CreateString(valueString));
    }
    else
    {
        cJSON_SetValuestring(o, valueString);
    }
}

static const char * contextGetString(cJSON * context, const char * string)
{
    cJSON * o = cJSON_GetObjectItem(context, string);
    if(o == NULL)
    {
        return "";
    }
    else
    {
        return cJSON_GetStringValue(o);
    }
}

static void contextSetCount(cJSON * context, int count)
{
    contextSetNumber(context, DICT_CONTEXT_COUNT, count);
}

static int contextGetCount(cJSON * context)
{
    return (int)contextGetNumber(context, DICT_CONTEXT_COUNT);
}

static void contextSetNrOfBlanks(cJSON * context, int nrOfBlanks)
{
    contextSetNumber(context, DICT_CONTEXT_NROFBLANKS, nrOfBlanks);
}

static int contextGetNrOfBlanks(cJSON * context)
{
    return contextGetNumber(context, DICT_CONTEXT_NROFBLANKS);
}

static void contextSetState(cJSON * context, int state)
{
    contextSetNumber(context, DICT_CONTEXT_STATE, state);
}

static int contextGetState(cJSON * context)
{
    return contextGetNumber(context, DICT_CONTEXT_STATE);
}

static void contextSetDataFile(cJSON * context, const char * file)
{
    contextSetString(context, DICT_CONTEXT_DATA_FILE, file);
}

static const char *  contextGetDataFile(cJSON * context)
{
    return contextGetString(context, DICT_CONTEXT_DATA_FILE);
}

static void contextSetSingleMeasurement(cJSON * context, const char * string, const SingleMeasurement_t * singleMeasurement)
{
    cJSON * oData = cJSON_GetObjectItem(context, DICT_CONTEXT_DATA);
    if(oData == NULL)
    {
        oData = cJSON_CreateObject();
        cJSON_AddItemToObject(context, DICT_CONTEXT_DATA, oData);
    }

    cJSON * oSingleMeasurement = singleMeasurement_toJson(singleMeasurement);

    cJSON * o = cJSON_GetObjectItem(oData, string);
    if(o == NULL)
    {
        o = cJSON_CreateObject();
        cJSON_AddItemToObject(oData, string, o);
        cJSON_AddItemToObject(o, string, oSingleMeasurement);
    }
    else
    {
        cJSON_ReplaceItemInObject(o, string, oSingleMeasurement);
    }
}

static void contextGetSingleMeasurement(cJSON * context, const char * string, SingleMeasurement_t * singleMeasurement)
{
    cJSON * oData = cJSON_GetObjectItem(context, DICT_CONTEXT_DATA);
    if(oData == NULL)
    {
        return;
    }

    cJSON * o = cJSON_GetObjectItem(oData, string);
    if(o == NULL)
    {
        return;
    }

    cJSON * oSingleMeasurement = cJSON_GetObjectItem(o, string);
    singleMeasurement_fromJson(oSingleMeasurement, singleMeasurement);
}

static void dataAddMeasurement(Evi_t* self, cJSON * context, const SingleMeasurement_t * baseline, const SingleMeasurement_t * air, const SingleMeasurement_t * sample, const char * comment, bool append)
{
    const char * file = contextGetDataFile(context);
    cJSON * data = dataLoadJson(self, file, append);

    cJSON* oMeasurements = cJSON_GetObjectItem(data, DICT_MEASUREMENTS);
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, DICT_BASELINE, singleMeasurement_toJson(baseline));
    cJSON_AddItemToObject(obj, DICT_AIR, singleMeasurement_toJson(air));
    cJSON_AddItemToObject(obj, DICT_SAMPLE, singleMeasurement_toJson(sample));

    {
        char * ts = malloc_timeStamp(TimeStampTypeISO8601);
        cJSON_AddItemToObject(obj, DICT_DATE_TIME, cJSON_CreateString(ts));
        free(ts);
    }

    cJSON * log = cJSON_CreateArray();

    char line[EVI_MAX_LINE_LENGTH];
    Error_t ret  = ERROR_EVI_OK;
    while(ret == ERROR_EVI_OK)
    {
        ret = eviLogging(self, line, sizeof(line));
        if(ret == ERROR_EVI_OK)
        {
            cJSON_AddItemToArray(log, cJSON_CreateString(line));
        }
    }

    if(log)
    {
        cJSON_AddItemToObject(obj, DICT_LOGGING, log);
    }

    if(comment)
    {
        cJSON_AddItemToObject(obj, DICT_COMMENT, cJSON_CreateString(comment));
    }

    cJSON_AddItemToArray(oMeasurements, obj);

    json_saveToFile(file, data);
    cJSON_Delete(data);
}

static char * createComment(cJSON * context)
{
    int count = contextGetCount(context);
    int nrOfBlanks = contextGetNrOfBlanks(context);

    if(count < nrOfBlanks)
    {
        return malloc_printf("Blank #%d", count + 1);
    }
    else
    {
        return malloc_printf("Sample #%d", count - nrOfBlanks + 1);
    }
}

static void reCalculate(cJSON * context, Options_t * options)
{
    cJSON *json = json_loadFromFile(contextGetDataFile(context));
    if (json != NULL)
    {
        cJSON *oMeasurements = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);
        Parameters_t  parameters = parametersCreate();
        parameters.blanksStart = contextGetNrOfBlanks(context);
        parameters.blanksEnd = 0;
        bool ret = measurement_calculate(oMeasurements, &parameters);
        if(ret == true)
        {
            json_saveToFile(contextGetDataFile(context), json);
        }
        cJSON_Delete(json);
    }
}

static Error_t measure(Evi_t* self, cJSON * context, Options_t * options, const char * comment)
{
    Error_t ret  = ERROR_EVI_OK;

    switch (contextGetState(context))
    {
        case StateBaseline:
        {
            SingleMeasurement_t baseline = {};
            ret = eviDenseBaseline(self, &baseline);
            if(ret == ERROR_EVI_OK)
            {
                contextSetSingleMeasurement(context, DICT_CONTEXT_DATA_BASELINE, &baseline);
                fprintf(stdout, "%i %i %i %i %i %i %i %i\n", baseline.channel230.sample, baseline.channel230.reference, baseline.channel260.sample, baseline.channel260.reference, baseline.channel280.sample, baseline.channel280.reference, baseline.channel340.sample, baseline.channel340.reference);
            }
            else
            {
                printError(ret, NULL);
            }
            contextAddLog(context, "measure() baseline ret:%i", ret);
            contextSetState(context, StateAir);
        }
        break;

        case StateAir:
        {
            SingleMeasurement_t air;
            ret = eviDenseMeasure(self, &air);
            if(ret == ERROR_EVI_OK)
            {
                contextSetSingleMeasurement(context, DICT_CONTEXT_DATA_AIR, &air);
                fprintf(stdout, "%i %i %i %i %i %i %i %i\n", air.channel230.sample, air.channel230.reference, air.channel260.sample, air.channel260.reference, air.channel280.sample, air.channel280.reference, air.channel340.sample, air.channel340.reference);
            }
            else
            {
                printError(ret, NULL);
            }
            contextAddLog(context, "measure() air ret:%i", ret);
            contextSetState(context, StateSample);
        }
        break;

        case StateSample:
        {
            SingleMeasurement_t baseline;
            SingleMeasurement_t air;
            SingleMeasurement_t sample;
            ret = eviDenseMeasure(self, &sample);

            contextGetSingleMeasurement(context, DICT_CONTEXT_DATA_BASELINE, &baseline);
            contextGetSingleMeasurement(context, DICT_CONTEXT_DATA_AIR, &air);
            if(ret == ERROR_EVI_OK)
            {
                char * _comment = NULL;
                if(comment == NULL)
                {
                    _comment = createComment(context);
                }

                dataAddMeasurement(self, context, &baseline, &air, &sample, comment ? comment : _comment, true);

                if(_comment != NULL)
                {
                    free(_comment);
                }

                fprintf(stdout, "%i %i %i %i %i %i %i %i\n", sample.channel230.sample, sample.channel230.reference, sample.channel260.sample, sample.channel260.reference, sample.channel280.sample, sample.channel280.reference, sample.channel340.sample, sample.channel340.reference);
            }
            else
            {
                printError(ret, NULL);
            }
            contextAddLog(context, "measure() sample ret:%i", ret);
            contextSetState(context, StateBaseline);
            contextSetCount(context, contextGetCount(context) + 1);
        }
        break;

        default:
            contextAddLog(context, "measure() wrong state:%i", contextGetState(context));
            break;
    }

    reCalculate(context, options);

    return ret;
}

Error_t cmdRun(Evi_t* self, int argcCmd, char** argvCmd)
{
    Error_t ret  = ERROR_EVI_OK;

    Options_t options = { 0 };

    int argcCmdSave = argcCmd;
    char **argvCmdSave = argvCmd;
    bool parsingOptions = true;
    size_t i = 1;

    while (i < argcCmd && parsingOptions)
    {
        if (strncmp(argvCmd[i], "--", 2) == 0 || strncmp(argvCmd[i], "-", 1) == 0)
        {
            #define WORKING_DIR "--working-dir="
			#define FILE_NAME   "--file="
            if (strncmp(argvCmd[i], WORKING_DIR, strlen(WORKING_DIR)) == 0)
            {
                const char * dir = argvCmd[i] + strlen(WORKING_DIR);
                chdir(dir);
            }
			else if (strncmp(argvCmd[i], FILE_NAME, strlen(FILE_NAME)) == 0)
			{
				options.filename_data = strdup(argvCmd[i] + strlen(FILE_NAME));
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

    if(options.filename_state == NULL)
    {
        Error_t ret = ERROR_EVI_OK;
        char    value[EVI_MAX_LINE_LENGTH];

        ret = eviGet(self, INDEX_SERIALNUMBER, value, sizeof(value));
        if (ret == ERROR_EVI_OK)
        {
            char * file = malloc_printf("evifluor-SN%s-state.json", value);
            options.filename_state = file;
        }
        else
        {
            options.filename_state = strdup("state.json");
        }
    }

    argcCmdSave = argcCmd - i;
    argvCmdSave = argvCmd + i;

    {
        cJSON * context = contextLoad(options.filename_state);

        if(argcCmdSave >= 1)
        {
            if(strcmp(argvCmdSave[0], "init") == 0)
            {
                if(argcCmdSave == 2)
                {
                    char sn[100] = {};
                    context = contextCreate(context);
                    contextSetNrOfBlanks(context, atoi(argvCmdSave[1]));
                    contextSetCount(context, 0);
                    contextSetState(context, StateBaseline);

                    if(options.filename_data == NULL)
                    {
                        if(eviGet(self, INDEX_SERIALNUMBER, sn, sizeof(sn)) == ERROR_EVI_OK)
                        {
                            char * ts   = malloc_timeStamp(TimeStampTypeFile);
                            char * file = malloc_printf("evidense-SN%s-%s.json", sn, ts);
                            contextSetDataFile(context, file);
                            free(ts);
                            free(file);
                        }
                        else
                        {
                            char * ts   = malloc_timeStamp(TimeStampTypeFile);
                            char * file = malloc_printf("evidense-SN%s-%s.json", "0", ts);
                            contextSetDataFile(context, file);
                            free(ts);
                            free(file);
                        }
                    }
                    else
                    {
                        contextSetDataFile(context, options.filename_data);
                    }
                    contextAddLog(context, "Created");
                    loggingClear(self);
                    fprintf_s(stdout, "Run initialized with %i blanks.\n", contextGetNrOfBlanks(context));
                    fprintf_s(stdout, "State stored in %s.\n", options.filename_state);
                    fprintf_s(stdout, "Data stored in %s.\n", contextGetDataFile(context));
                }
                else
                {
                    ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, NULL);
                }
            }
            else if(strcmp(argvCmdSave[0], "measure") == 0)
            {
                const char * comment = NULL;
                if(argcCmdSave >= 2)
                {
                    comment = argvCmdSave[1];
                }
                measure(self, context, &options, comment);
            }
            else if(strcmp(argvCmdSave[0], "checkempty") == 0)
            {
                bool empty = 0;
                ret = eviDenseIsCuvetteHolderEmpty(self, &empty);
                contextAddLog(context, "check empty ret:%i empty:%i", ret, empty);
                if(ret == ERROR_EVI_OK)
                {
                    if(empty == 0)
                    {
                        ret = ERROR_EVI_CUVETTE_GUIDE_NOT_EMPTY;
                    }
                }
            }
            else if(strcmp(argvCmdSave[0], "export") == 0)
            {
                ExportOptions_t options = {};
                options.delimiter = ';';
                options.mode      = MODE_MEASUREMENT;
                options.filenameJson = (char*)contextGetDataFile(context);
                options.filenameCsv  =  malloc_replace_suffix(options.filenameJson, "csv");
                ret = exportData(&options);
                free(options.filenameCsv);
            }
            else
            {
                ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, NULL);
            }
        }
        else
        {
            ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, NULL);
        }

        contextSave(context, options.filename_state);
        cJSON_Delete(context);
    }

exit:
    free(options.filename_state);
    free(options.filename_data);

    return ret;
}
