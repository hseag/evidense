// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"
#include "commonerror.h"
#include "commonindex.h"
#include "evidenseerror.h"
#include "evidenseindex.h"

typedef struct
{
    uint32_t result;
    uint32_t current;
    uint32_t amplificationSample;
    uint32_t amplificationReference;
} Levelling_t;

typedef struct
{
    uint32_t * sample230;
    uint32_t * reference230;
    uint32_t * sample260;
    uint32_t * reference260;
    uint32_t * sample280;
    uint32_t * reference280;
    uint32_t * sample340;
    uint32_t * reference340;
} UserMeasurement;


typedef struct
{
    Levelling_t * levelling230;
    Levelling_t * levelling260;
    Levelling_t * levelling280;
    Levelling_t * levelling340;
} UserLevelling;

typedef struct
{
    uint32_t * voltageSample;
    uint32_t * voltageReference;
    uint32_t * voltage5V;
    uint32_t * current;
} UserADC;

typedef enum
{
    SETUPRESULT_OK = 0,
    SETUPRESULT_AMPLIFICATION_NOT_FOUND = 1,
    SETUPRESULT_REFERENCE_CHANNEL_TOOH_IGH = 2,
    SETUPRESULT_SAMPLE_CHANNEL_TOO_HIGH = 3,
} SetupResult_t;

typedef enum
{
    ERROR_EVI_LEVELLING_FAILED = ERROR_EVI_USER
} ErrorEviDense_t;


#define SELFTEST_ILED_230      0x00000001
#define SELFTEST_ILED_260      0x00000002
#define SELFTEST_ILED_280      0x00000004
#define SELFTEST_ILED_340      0x00000008
#define SELFTEST_SAMPLE_230    0x00000010
#define SELFTEST_SAMPLE_260    0x00000020
#define SELFTEST_SAMPLE_280    0x00000040
#define SELFTEST_SAMPLE_340    0x00000080
#define SELFTEST_REFERENCE_230 0x00000100
#define SELFTEST_REFERENCE_260 0x00000200
#define SELFTEST_REFERENCE_280 0x00000400
#define SELFTEST_REFERENCE_340 0x00000800
#define SELFTEST_REFERENCE     0x00001000
#define SELFTEST_SAMPLE        0x00002000

DLLEXPORT Error_t eviDenseMeasure(Evi_t *self, uint32_t *sample230, uint32_t *reference230, uint32_t *sample260, uint32_t *reference260, uint32_t *sample280, uint32_t *reference280, uint32_t *sample340, uint32_t *reference340);
DLLEXPORT Error_t eviDenseBaseline(Evi_t *self, uint32_t *sample230, uint32_t *reference230, uint32_t *sample260, uint32_t *reference260, uint32_t *sample280, uint32_t *reference280, uint32_t *sample340, uint32_t *reference340);
DLLEXPORT Error_t eviDenseLevelling(Evi_t *self, Levelling_t *levelling230, Levelling_t *levelling260, Levelling_t *levelling280, Levelling_t *levelling340);
DLLEXPORT Error_t eviDenseLastMeasurements(Evi_t *self, uint32_t last, uint32_t *sample230, uint32_t *reference230, uint32_t *sample260, uint32_t *reference260, uint32_t *sample280, uint32_t *reference280, uint32_t *sample340, uint32_t *reference340);
DLLEXPORT Error_t eviDenseLastLevelling(Evi_t *self, Levelling_t *levelling230, Levelling_t *levelling260, Levelling_t *levelling280, Levelling_t *levelling340);

