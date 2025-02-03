/*
 * Copyright (C) 2025 Hombrechtikon Systems Engineering AG
 */

#pragma once

enum EviDenseIndices
{
    INDEX_LAST_MEASUREMENT_COUNT                  = 10,

    INDEX_LED230NM_MAX_CURRENT                    = 23,
    INDEX_LED260NM_MAX_CURRENT                    = 33,
    INDEX_LED280NM_MAX_CURRENT                    = 43,
    INDEX_LED340NM_MAX_CURRENT                    = 53,

    INDEX_AMPLIFIER_SAMPLEFACTOR___1_1            = 60,
    INDEX_AMPLIFIER_SAMPLEFACTOR__11_0            = 61,
    INDEX_AMPLIFIER_SAMPLEFACTOR_111_0            = 62,
    INDEX_AMPLIFIER_REFERENCEFACTOR___1_1         = 63,
    INDEX_AMPLIFIER_REFERENCEFACTOR__11_0         = 64,
    INDEX_AMPLIFIER_REFERENCEFACTOR_111_0         = 65,

    INDEX_AUTO_MEASURE_T1                         = 70,
    INDEX_AUTO_MEASURE_T2                         = 71,
    INDEX_AUTO_MEASURE_T3                         = 72,

    INDEX_SETUP_TARGET230                         = 80,
    INDEX_SETUP_TARGET260                         = 81,
    INDEX_SETUP_TARGET280                         = 82,
    INDEX_SETUP_TARGET340                         = 83,
    INDEX_CURRENT_ON_WAIT_TIME                    = 84,
    INDEX_CURRENT_OFF_WAIT_TIME                   = 85,
    INDEX_NUMBER_OF_SAMPLES                       = 86,
    
    INDEX_CUVETTE_EMPTY_SPLIT_RATIO_THRESHOLD     = 90,

    INDEX_SELFTEST_AMPLIFER_SPLITRATIO230NM       = 100,
    INDEX_SELFTEST_AMPLIFER_CURRENT               = 101,
    INDEX_SELFTEST_AMPLIFER_SAMPLE1               = 102,
    INDEX_SELFTEST_AMPLIFER_SAMPLE11              = 103,
    INDEX_SELFTEST_AMPLIFER_SAMPLE111             = 104,
    INDEX_SELFTEST_AMPLIFER_REFERENCE1            = 105,
    INDEX_SELFTEST_AMPLIFER_REFERENCE11           = 106,
    INDEX_SELFTEST_AMPLIFER_REFERENCE111          = 107,
    INDEX_SELFTEST_AMPLIFER_SETUPRESULT           = 108,

    INDEX_SELFTEST_LED230_ILED                    = 110,
    INDEX_SELFTEST_LED230_DARKSAMPLE              = 111,
    INDEX_SELFTEST_LED230_DARKREFERENCE           = 112,
    INDEX_SELFTEST_LED230_SAMPLE                  = 113,
    INDEX_SELFTEST_LED230_REFERENCE               = 114,

    INDEX_SELFTEST_LED260_ILED                    = 120,
    INDEX_SELFTEST_LED260_DARKSAMPLE              = 121,
    INDEX_SELFTEST_LED260_DARKREFERENCE           = 122,
    INDEX_SELFTEST_LED260_SAMPLE                  = 123,
    INDEX_SELFTEST_LED260_REFERENCE               = 124,

    INDEX_SELFTEST_LED280_ILED                    = 130,
    INDEX_SELFTEST_LED280_DARKSAMPLE              = 131,
    INDEX_SELFTEST_LED280_DARKREFERENCE           = 132,
    INDEX_SELFTEST_LED280_SAMPLE                  = 133,
    INDEX_SELFTEST_LED280_REFERENCE               = 134,

    INDEX_SELFTEST_LED340_ILED                    = 140,
    INDEX_SELFTEST_LED340_DARKSAMPLE              = 141,
    INDEX_SELFTEST_LED340_DARKREFERENCE           = 142,
    INDEX_SELFTEST_LED340_SAMPLE                  = 143,
    INDEX_SELFTEST_LED340_REFERENCE               = 144,

    INDEX_LEVELLING_LED230_SETUPRESULT            = 150,
    INDEX_LEVELLING_LED230_CURRENT                = 151,
    INDEX_LEVELLING_LED230_AMPLIFICATIONSAMPLE    = 152,
    INDEX_LEVELLING_LED230_AMPLIFICATIONREFERENCE = 153,

    INDEX_LEVELLING_LED260_SETUPRESULT            = 160,
    INDEX_LEVELLING_LED260_CURRENT                = 161,
    INDEX_LEVELLING_LED260_AMPLIFICATIONSAMPLE    = 162,
    INDEX_LEVELLING_LED260_AMPLIFICATIONREFERENCE = 163,

    INDEX_LEVELLING_LED280_SETUPRESULT            = 170,
    INDEX_LEVELLING_LED280_CURRENT                = 171,
    INDEX_LEVELLING_LED280_AMPLIFICATIONSAMPLE    = 172,
    INDEX_LEVELLING_LED280_AMPLIFICATIONREFERENCE = 173,

    INDEX_LEVELLING_LED340_SETUPRESULT            = 180,
    INDEX_LEVELLING_LED340_CURRENT                = 181,
    INDEX_LEVELLING_LED340_AMPLIFICATIONSAMPLE    = 182,
    INDEX_LEVELLING_LED340_AMPLIFICATIONREFERENCE = 183,

    //USED ONLY FROM DILBERT
    INDEX_VOLTAGE_SAMPLE                          = 1000,
    INDEX_VOLTAGE_REFERNECE                       = 1001,
    INDEX_VOLTAGE_REF                             = 1002,
    INDEX_CURRENT_LED                             = 1003,

    INDEX_VOLTAGE_SAMPLE_MIN                      = 2000,
    INDEX_VOLTAGE_SAMPLE_MAX                      = 2001,
    INDEX_VOLTAGE_SAMPLE_DELTA                    = 2002,
    INDEX_VOLTAGE_REFERENCE_MIN                   = 2003,
    INDEX_VOLTAGE_REFERENCE_MAX                   = 2004,
    INDEX_VOLTAGE_REFERENCE_DELTA                 = 2005,

    INDEX_VOLTAGE_REF_MIN                         = 0206,
    INDEX_VOLTAGE_REF_MAX                         = 2007,
    INDEX_VOLTAGE_REF_DELTA                       = 2008,

    INDEX_LED230NM_AMPLIFICATIONSAMPLE            = 2010,
    INDEX_LED230NM_AMPLIFICATIONREFERENCE         = 2011,
    INDEX_LED230NM_CURRENT                        = 2012,
    INDEX_LED230NM_SETUPRESULT                    = 2013,
                                                  
    INDEX_LED260NM_AMPLIFICATIONSAMPLE            = 2020,
    INDEX_LED260NM_AMPLIFICATIONREFERENCE         = 2021,
    INDEX_LED260NM_CURRENT                        = 2022,
    INDEX_LED260NM_SETUPRESULT                    = 2023,
                                                  
    INDEX_LED280NM_AMPLIFICATIONSAMPLE            = 2030,
    INDEX_LED280NM_AMPLIFICATIONREFERENCE         = 2031,
    INDEX_LED280NM_CURRENT                        = 2032,
    INDEX_LED280NM_SETUPRESULT                    = 2033,
                                                  
    INDEX_LED340NM_AMPLIFICATIONSAMPLE            = 2040,
    INDEX_LED340NM_AMPLIFICATIONREFERENCE         = 2041,
    INDEX_LED340NM_CURRENT                        = 2042,
    INDEX_LED340NM_SETUPRESULT                    = 2043,

    INDEX_CURRENT_LED_MIN                         = 2050,
    INDEX_CURRENT_LED_MAX                         = 2051,
    INDEX_CURRENT_LED_DELTA                       = 2052,

    INDEX_CUVETTE_EMPTY_IS_EMPTY                  = 2060,
    INDEX_CUVETTE_EMPTY_SPLIT_RATIO_MEASURED      = 2061,

    INDEX_DEMO_MODE_STATE                         = 3000,
};
