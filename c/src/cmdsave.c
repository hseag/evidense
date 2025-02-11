// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cmdsave.h"
#include "json.h"
#include "evidense.h"
#include "commonindex.h"
#include "evidenseindex.h"
#include "printerror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


typedef struct
{
    bool append;
    bool raw;
    char * filename;
    char * comment;
} Options_t;

static double amplification2number(Evi_t* self, bool isSample, uint32_t value)
{
    int  index;
    char buffer[100];

    if (isSample)
    {
        switch (value)
        {
            case 0:
                index = INDEX_AMPLIFIER_SAMPLEFACTOR___1_1;
                break;
            case 1:
                index = INDEX_AMPLIFIER_SAMPLEFACTOR__11_0;
                break;
            case 2:
                index = INDEX_AMPLIFIER_SAMPLEFACTOR_111_0;
                break;
            default:
                return 0.0;
        }
    }
    else
    {
        switch (value)
        {
            case 0:
                index = INDEX_AMPLIFIER_REFERENCEFACTOR___1_1;
                break;
            case 1:
                index = INDEX_AMPLIFIER_REFERENCEFACTOR__11_0;
                break;
            case 2:
                index = INDEX_AMPLIFIER_REFERENCEFACTOR_111_0;
                break;
            default:
                return 0.0;
        }
    }

    if (eviGet(self, index, buffer, sizeof(buffer)) != ERROR_EVI_OK)
    {
        return 0.0;
    }

    return atof(buffer);
}

static char* result2text(uint32_t value)
{
    switch (value)
    {
        case SETUPRESULT_OK:
            return "OK";
        case SETUPRESULT_AMPLIFICATION_NOT_FOUND:
            return "No amplification found";
        case SETUPRESULT_REFERENCE_CHANNEL_TOOH_IGH:
            return "Reference channel too high";
        case SETUPRESULT_SAMPLE_CHANNEL_TOO_HIGH:
            return "Sample channel too high";
        default:
            return "?";
    }
}

static cJSON* channelToJson(const Channel_t * channel)
{
    cJSON* obj        = cJSON_CreateObject();
    cJSON* oSample    = cJSON_CreateNumber(channel->sample);
    cJSON* oReference = cJSON_CreateNumber(channel->reference);

    cJSON_AddItemToObject(obj, DICT_SAMPLE, oSample);
    cJSON_AddItemToObject(obj, DICT_REFERENCE, oReference);

    return obj;
}

static cJSON* measurementToJson(const SingleMeasurement_t * measurement)
{
    cJSON* obj = cJSON_CreateObject();

    cJSON_AddItemToObject(obj, DICT_230, channelToJson(&measurement->channel230));
    cJSON_AddItemToObject(obj, DICT_260, channelToJson(&measurement->channel260));
    cJSON_AddItemToObject(obj, DICT_280, channelToJson(&measurement->channel280));
    cJSON_AddItemToObject(obj, DICT_340, channelToJson(&measurement->channel340));

    return obj;
}

static Error_t addSingleMeasurement(Evi_t* self, char* key, int index, cJSON* obj)
{
    Error_t  ret = ERROR_EVI_OK;

    SingleMeasurement_t measuremnt = {0};

    ret = eviDenseLastMeasurements(self, index, &measuremnt);
    if (ret == ERROR_EVI_OK)
    {
        if(key == NULL)
        {
            cJSON_AddItemToArray(obj, measurementToJson(&measuremnt));
        }
        else
        {
            cJSON_AddItemToObject(obj, key, measurementToJson(&measuremnt));
        }
        return ret;
    }
    else
    {
        printError(ret, "Could not read measurement");
        return ret;
    }
}

static cJSON* levellingChannel(Evi_t* self, Levelling_t levelling)
{
    cJSON* obj = cJSON_CreateObject();

    cJSON_AddItemToObject(obj, DICT_AMPLIFICATION_SAMPLE, cJSON_CreateNumber(amplification2number(self, true, levelling.amplificationSample)));
    cJSON_AddItemToObject(obj, DICT_AMPLIFICATION_REFERENCE, cJSON_CreateNumber(amplification2number(self, false, levelling.amplificationReference)));
    cJSON_AddItemToObject(obj, DICT_CURRENT, cJSON_CreateNumber(levelling.current));
    cJSON_AddItemToObject(obj, DICT_RESULT, cJSON_CreateNumber(levelling.result));
    cJSON_AddItemToObject(obj, DICT_RESULT_TEXT, cJSON_CreateString(result2text(levelling.result)));

    return obj;
}

static Error_t addLevelling(Evi_t* self, cJSON* obj)
{
    Error_t     ret = ERROR_EVI_OK;
    Levelling_t levelling230;
    Levelling_t levelling260;
    Levelling_t levelling280;
    Levelling_t levelling340;

    ret = eviDenseLastLevelling(self, &levelling230, &levelling260, &levelling280, &levelling340);
    if (ret != ERROR_EVI_OK)
    {
        printError(ret, "Could not read levelling info");
        return ret;
    }

    cJSON* objLevelling = cJSON_CreateObject();

    cJSON_AddItemToObject(objLevelling, DICT_230, levellingChannel(self, levelling230));
    cJSON_AddItemToObject(objLevelling, DICT_260, levellingChannel(self, levelling260));
    cJSON_AddItemToObject(objLevelling, DICT_280, levellingChannel(self, levelling280));
    cJSON_AddItemToObject(objLevelling, DICT_340, levellingChannel(self, levelling340));

    cJSON_AddItemToObject(obj, DICT_LEVELLING, objLevelling);

    return ERROR_EVI_OK;
}

static Error_t addMeasurement(Evi_t* self, Options_t * options, cJSON* json)
{
    Error_t ret = ERROR_EVI_OK;
    char    value[20];

    cJSON* oMeasurements = cJSON_GetObjectItem(json, DICT_MEASUREMENTS);

    cJSON* obj = cJSON_CreateObject();

    if (options->comment != NULL)
    {
        cJSON_AddItemToObject(obj, DICT_COMMENT, cJSON_CreateString(options->comment));
    }

    ret = eviGet(self, INDEX_LAST_MEASUREMENT_COUNT, value, sizeof(value));
    if (ret != ERROR_EVI_OK)
    {
        printError(ret, "Could not read measurement count");
        return ret;
    }

    ret = addLevelling(self, obj);
    if (ret != ERROR_EVI_OK)
        return ret;

    int lastMeasurementsCount = atoi(value);
    if (lastMeasurementsCount == 2 && options->raw == false)
    {
        ret = addSingleMeasurement(self, DICT_BASELINE, 1, obj);
        if (ret != ERROR_EVI_OK)
            return ret;

        ret = addSingleMeasurement(self, DICT_SAMPLE, 0, obj);
        if (ret != ERROR_EVI_OK)
            return ret;
    }
    else if (lastMeasurementsCount == 3 && options->raw == false)
    {
        ret = addSingleMeasurement(self, DICT_BASELINE, 2, obj);
        if (ret != ERROR_EVI_OK)
            return ret;

        ret = addSingleMeasurement(self, DICT_AIR, 1, obj);
        if (ret != ERROR_EVI_OK)
            return ret;

        ret = addSingleMeasurement(self, DICT_SAMPLE, 0, obj);
        if (ret != ERROR_EVI_OK)
            return ret;
    }
    else
    {
        cJSON* oValues = cJSON_CreateArray();
        cJSON_AddItemToObject(obj, DICT_VALUES, oValues);

        for(int i = lastMeasurementsCount-1; i>=0; i--)
        {
            ret = addSingleMeasurement(self, NULL, i, oValues);
            if (ret != ERROR_EVI_OK)
                return ret;
        }
    }

    cJSON_AddItemToArray(oMeasurements, obj);

    return ERROR_EVI_OK;
}

static cJSON* loadJson(Evi_t* self, Options_t * options)
{
    cJSON* json = NULL;

    if(options->append == true)
    {
        json = jsonLoad(options->filename);
    }

    // create new JSON file
    if (json == NULL)
    {
        Error_t ret = ERROR_EVI_OK;
        char    value[100];

        json = cJSON_CreateObject();

        ret = eviGet(self, INDEX_SERIALNUMBER, value, sizeof(value));
        if (ret == ERROR_EVI_OK)
        {
            cJSON_AddItemToObject(json, DICT_SERIALNUMBER, cJSON_CreateString(value));
        }

        ret = eviGet(self, INDEX_VERSION, value, sizeof(value));
        if (ret == ERROR_EVI_OK)
        {
            cJSON_AddItemToObject(json, DICT_FIRMWAREVERSION, cJSON_CreateString(value));
        }

        cJSON_AddItemToObject(json, DICT_MEASUREMENTS, cJSON_CreateArray());
    }

    return json;
}

Error_t cmdSave(Evi_t* self, int argcCmd, char** argvCmd)
{
    Error_t ret  = ERROR_EVI_OK;
    cJSON*  json = NULL;
    Options_t options = { 0 };

    options.append = true;

	int argcCmdSave = argcCmd;
	char **argvCmdSave = argvCmd;
    bool parsingOptions = true;
    size_t i = 1;
    
    while (i < argcCmd && parsingOptions)
	{
		if (strncmp(argvCmd[i], "--", 2) == 0 || strncmp(argvCmd[i], "-", 1) == 0)
		{
			if (strcmp(argvCmd[i], "--append") == 0)
			{
                options.append = true;
			}
            else if (strcmp(argvCmd[i], "--create") == 0)
            {
                options.append = false;
            }
            else if (strcmp(argvCmd[i], "--mode-raw") == 0)
            {
                options.raw = true;
            }
            else if (strcmp(argvCmd[i], "--mode-measurement") == 0)
            {
                options.raw = false;
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

    if(argcCmdSave == 1)
    {
        options.filename = strdup(argvCmdSave[0]);
    }
    else if(argcCmdSave == 2)
    {
        options.filename = strdup(argvCmdSave[0]);
        options.comment  = strdup(argvCmdSave[1]);
    }
    else
    {
        ret = printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, NULL);
        goto exit;
    }

    json = loadJson(self, &options);
    ret  = addMeasurement(self, &options, json);
    if (ret == ERROR_EVI_OK)
    {
        jsonSave(options.filename, json);
    }
exit:
    if (json)
        cJSON_Delete(json);
    free(options.filename);
    free(options.comment);

    return ret;
}
