// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "measurement.h"
#include "evibase.h"
#include <string.h>
#include <math.h>

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

Quadruple_t measurement_factorAirToBlank(const Measurement_t * self)
{
    Quadruple_t a  = singleMeasurement_sample(&self->sample);
    Quadruple_t b  = singleMeasurement_reference(&self->sample);
    Quadruple_t c  = singleMeasurement_reference(&self->air);
    Quadruple_t d  = singleMeasurement_sample(&self->air);
    Quadruple_t ab = quadruple_div(&a, &b);
    Quadruple_t cd = quadruple_div(&c, &d);

    Quadruple_t ret = quadruple_mul(&ab, &cd);
    return ret;
}

Quadruple_t measurement_factor340ToNNN(const Measurement_t * self)
{
    Quadruple_t aNNN = measurement_calculateAbsorbance(&self->air, &self->sample, NULL);
    Quadruple_t a340 = quadruple_initAllTheSame(aNNN.value340);
    return quadruple_div(&a340, &aNNN);
}

Quadruple_t measurement_algorithmV7(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN     = measurement_calculateAbsorbance(&self->air, &self->sample, factorAirToBlank);
    Quadruple_t aNNN_340 = quadruple_initAllTheSame(aNNN.value340);
    Quadruple_t b        = quadruple_mul(&aNNN_340, factor340ToNNN);
    Quadruple_t ret      = quadruple_sub(&aNNN, &b);

    return ret;
}

double measurement_dsDNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN = measurement_algorithmV7(self, factorAirToBlank, factor340ToNNN);
    return aNNN.value260 * 50 * 10 / 1; //50ng/ul * 10mm / 1mm
}

double measurement_ssDNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN = measurement_algorithmV7(self, factorAirToBlank, factor340ToNNN);
    return aNNN.value260 * 33 * 10 / 1; //33ng/ul * 10mm / 1mm
}

double measurement_ssRNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN = measurement_algorithmV7(self, factorAirToBlank, factor340ToNNN);
    return aNNN.value260 * 40 * 10 / 1; //40ng/ul * 10mm / 1mm
}

double measurement_purityRatio260_280(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN = measurement_algorithmV7(self, factorAirToBlank, factor340ToNNN);
    return  aNNN.value260 / aNNN.value280;
}

double measurement_purityRatio260_230(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN)
{
    Quadruple_t aNNN = measurement_algorithmV7(self, factorAirToBlank, factor340ToNNN);
    return  aNNN.value260 / aNNN.value230;
}

