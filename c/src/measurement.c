// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evibase.h"
#include "dict.h"
#include "measurement.h"
#include <string.h>
#include <math.h>

#define DEFAULT_CUVETTE_PATH_LENGTH 1.1 //mm

Measurement_t measurement_init(SingleMeasurement_t baseline, SingleMeasurement_t air, SingleMeasurement_t sample, const char * comment)
{
    Measurement_t ret = {.baseline = baseline, .air = air, .sample = sample, .comment = {0}};

    if(comment)
    {
        strncpy_s(ret.comment, MAX_COMMENT_LENGTH, comment, MAX_COMMENT_LENGTH);
    }

    return ret;
}

void measurement_print(const Measurement_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "baseline: ");
    singleMeasurement_print(&self->baseline, stream, false);
    fprintf_s(stream, " ");
    fprintf_s(stream, "air: ");
    singleMeasurement_print(&self->air, stream, false);
    fprintf_s(stream, " ");
    fprintf_s(stream, "sample: ");
    singleMeasurement_print(&self->sample, stream, false);
    fprintf_s(stream, "%s", newLine ? "\n" : "");
}

Quadruple_t measurement_calculateAbsorbance(const SingleMeasurement_t * baseline, const SingleMeasurement_t * measurement, const Quadruple_t * correctionFactor)
{
    Quadruple_t ods;
    Quadruple_t  correctionFactorOne = quadruple_initAllTheSame(1.0);

    if(!correctionFactor)
    {
        correctionFactor = &correctionFactorOne;
    }

    ods.value230 = log10((double)baseline->channel230.sample / (double)baseline->channel230.reference * (double)measurement->channel230.reference / (double)measurement->channel230.sample * correctionFactor->value230);
    ods.value260 = log10((double)baseline->channel260.sample / (double)baseline->channel260.reference * (double)measurement->channel260.reference / (double)measurement->channel260.sample * correctionFactor->value260);
    ods.value280 = log10((double)baseline->channel280.sample / (double)baseline->channel280.reference * (double)measurement->channel280.reference / (double)measurement->channel280.sample * correctionFactor->value280);
    ods.value340 = log10((double)baseline->channel340.sample / (double)baseline->channel340.reference * (double)measurement->channel340.reference / (double)measurement->channel340.sample * correctionFactor->value340);

    return ods;
}

Quadruple_t measurement_factorAbsorbanceBufferBlank(const Measurement_t * self)
{
    return measurement_calculateAbsorbance(&self->air, &self->sample, NULL);
}

Quadruple_t measurement_algorithmV9(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank)
{
    Quadruple_t aSample           = measurement_calculateAbsorbance(&self->air, &self->sample,NULL);
    Quadruple_t aSampleMinusBlank = quadruple_sub(&aSample, factorAbsorbanceBufferBlank);
    Quadruple_t a340              = quadruple_initAllTheSame(aSampleMinusBlank.value340);
    Quadruple_t ret               = quadruple_sub(&aSampleMinusBlank, &a340);

    return ret;
}

double measurement_dsDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank);
    return aNNN.value260 * 50.0 * 10.0 / tempCcuvettePathLength; //50ng/ul * 10mm / 1.1mm
}

double measurement_ssDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank);
    return aNNN.value260 * 33.0 * 10.0 / tempCcuvettePathLength; //33ng/ul * 10mm / 1.1mm
}

double measurement_ssRNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank);
    return aNNN.value260 * 40.0 * 10.0 / tempCcuvettePathLength; //40ng/ul * 10mm / 1.1mm
}

double measurement_purityRatio260_280(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank)
{
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank);
    return  aNNN.value260 / aNNN.value280;
}

double measurement_purityRatio260_230(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank)
{
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank);
    return  aNNN.value260 / aNNN.value230;
}

bool measurement_fromJson(cJSON * node, Measurement_t * measurement)
{
    bool ret = false;
    cJSON * oBaseline = cJSON_GetObjectItem(node, DICT_BASELINE);
    cJSON * oAir      = cJSON_GetObjectItem(node, DICT_AIR);
    cJSON * oSample   = cJSON_GetObjectItem(node, DICT_SAMPLE);
    if(oBaseline && oAir && oSample)
    {
        SingleMeasurement_t baseline = {};
        SingleMeasurement_t air = {};
        SingleMeasurement_t sample = {};
        if(singleMeasurement_fromJson(oBaseline, &baseline) && singleMeasurement_fromJson(oAir, &air) && singleMeasurement_fromJson(oSample, &sample))
        {
            *measurement = measurement_init(baseline, air, sample, NULL);
            ret = true;
        }
    }

    return ret;
}

Parameters_t parametersCreate()
{
    Parameters_t ret;

    ret.blanksStart       = 1;
    ret.blanksEnd         = 0;
    ret.cuvettePathLength = 1.1; //mm

    return ret;
}

bool measurement_calculateFactors(cJSON *oMeasurments, const Parameters_t * parameters, Factors_t * factors)
{
    bool ret = false;
    cJSON *iterator = NULL;
    uint32_t index = 0;
    uint32_t count = 0;

    uint32_t length = cJSON_GetArraySize(oMeasurments);

    factors->fAbsorbanceBufferBlank = quadruple_initAllTheSame(0.0);

    cJSON_ArrayForEach(iterator, oMeasurments)
    {
        if ((index < parameters->blanksStart) || (index >= (length - parameters->blanksEnd)))
        {
            Measurement_t m = {};
            measurement_fromJson(iterator, &m);
            Quadruple_t f1  = measurement_factorAbsorbanceBufferBlank(&m);
            factors->fAbsorbanceBufferBlank = quadruple_add(&factors->fAbsorbanceBufferBlank, &f1);
            count++;
        }
        index++;
    }

    if (count == 0)
    {
        factors->fAbsorbanceBufferBlank = quadruple_initAllTheSame(1.0);
    }
    else
    {
        Quadruple_t qCount       = quadruple_initAllTheSame(count);

        factors->fAbsorbanceBufferBlank = quadruple_div(&factors->fAbsorbanceBufferBlank, &qCount);
        ret = count == (parameters->blanksStart + parameters->blanksEnd);
    }
    return ret;
}

static cJSON *calculate(cJSON *measurement, const Factors_t * factors, const Parameters_t * parameters)
{
    cJSON *obj = cJSON_CreateObject();

    Measurement_t m = {};

    measurement_fromJson(measurement, &m);

    cJSON_AddNumberToObject(obj, DICT_DS_DNA, measurement_dsDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));
    cJSON_AddNumberToObject(obj, DICT_SS_DNA, measurement_ssDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));
    cJSON_AddNumberToObject(obj, DICT_SS_RNA, measurement_ssRNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength));

    cJSON_AddNumberToObject(obj, DICT_PURITY_260_230, measurement_purityRatio260_230(&m, &factors->fAbsorbanceBufferBlank));
    cJSON_AddNumberToObject(obj, DICT_PURITY_260_280, measurement_purityRatio260_280(&m, &factors->fAbsorbanceBufferBlank));

    return obj;
}

bool measurement_calculate(cJSON * oMeasurements, const Parameters_t * parameters)
{
    bool ret = false;

    if (oMeasurements)
    {
        Factors_t factors = {};

        if(measurement_calculateFactors(oMeasurements, parameters, &factors))
        {
            cJSON *iterator = NULL;
            cJSON_ArrayForEach(iterator, oMeasurements)
            {
                cJSON_DeleteItemFromObject(iterator, DICT_CALCULATED);
                cJSON_AddItemToObject(iterator, DICT_CALCULATED, calculate(iterator, &factors, parameters));
            }
            ret = true;
        }
    }
    return ret;
}
