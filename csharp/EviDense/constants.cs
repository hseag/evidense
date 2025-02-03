// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;

namespace Hse.EviDense;

/// <summary>
/// Provides USB-related constants.
/// </summary>
public static class USB
{
    /// <summary>
    /// Vendor ID (VID) of the USB device.
    /// </summary>
    public const int VID = 0x1CBE; // 7358

    /// <summary>
    /// Product ID (PID) of the USB device.
    /// </summary>
    public const int PID = 2;
}

/// <summary>
/// Defines constant dictionary keys used in measurement processing.
/// </summary>
public class Dict
{
    public const string BASELINE = "baseline";
    public const string AIR = "air";
    public const string SAMPLE = "sample";
    public const string REFERENCE = "reference";
    public const string COMMENT = "comment";
    public const string CH_230 = "230";
    public const string CH_260 = "260";
    public const string CH_280 = "280";
    public const string CH_340 = "340";
    public const string dsDNA = "dsDNA";
    public const string ssDNA = "ssDNA";
    public const string ssRNA = "ssRNA";
    public const string PURITY260_280 = "purity260/280";
    public const string PURITY260_230 = "purity260/230";
    public const string MEASUREMENTS = "measurements";
    public const string VALUES = "values";
    public const string RESULTS = "results";
    public const string SERIALNUMBER = "serialnumber";
    public const string FIRMWAREVERSION = "firmwareVersion";
    public const string LEVELLING = "levelling";
    public const string LEVELLING_RESULT = "result";
    public const string LEVELLING_CURRENT = "current";
    public const string LEVELLING_AMPLIFIERSAMPLE = "amplifiersample";
    public const string LEVELLING_AMPLIFIERREFERENCE = "amplifierreference";
    public const string SELFTEST = "selftest";
    public const string SELFTEST_RESULT = "result";
    public const string SELFTEST_SPLITRATION230NM = "splitration230nm";
    public const string SELFTEST_AMPLIFIERS = "amplifiers";
    public const string SELFTEST_AMPLIFIER_CURRENT = "current";
    public const string SELFTEST_AMPLIFIER_SAMPLE1 = "sample1";
    public const string SELFTEST_AMPLIFIER_SAMPLE11 = "sample11";
    public const string SELFTEST_AMPLIFIER_SAMPLE111 = "sample111";
    public const string SELFTEST_AMPLIFIER_REFERENCE1 = "reference1";
    public const string SELFTEST_AMPLIFIER_REFERENCE11 = "reference11";
    public const string SELFTEST_AMPLIFIER_REFERENCE111 = "reference111";
    public const string SELFTEST_ILED = "iled";
    public const string SELFTEST_DARKSAMPLE = "darksample";
    public const string SELFTEST_DARKREFERENCE = "darkreference";
    public const string SELFTEST_SAMPLE = "sample";
    public const string SELFTEST_REFERENCE = "reference";
}

/// <summary>
/// Enumeration representing different index values for hardware configurations.
/// </summary>
public enum Index
{
    VERSION = 0,
    SERIALNUMBER = 1,
    HARDWARETYPE = 2,
    LAST_MEASUREMENT_COUNT = 10,
    LED230NM_MAX_CURRENT = 23,
    LED260NM_MAX_CURRENT = 33,
    LED280NM_MAX_CURRENT = 43,
    LED340NM_MAX_CURRENT = 53,
    AMPLIFIER_SAMPLEFACTOR___1_1 = 60,
    AMPLIFIER_SAMPLEFACTOR__11_0 = 61,
    AMPLIFIER_SAMPLEFACTOR_111_0 = 62,
    AMPLIFIER_REFERENCEFACTOR___1_1 = 63,
    AMPLIFIER_REFERENCEFACTOR__11_0 = 64,
    AMPLIFIER_REFERENCEFACTOR_111_0 = 65,
    SETUP_TARGET230 = 80,
    SETUP_TARGET260 = 81,
    SETUP_TARGET280 = 82,
    SETUP_TARGET340 = 83,

    SELFTEST_AMPLIFIER_SPLITRATIO230NM = 100,
    SELFTEST_AMPLIFIER_CURRENT = 101,
    SELFTEST_AMPLIFIER_SAMPLE1 = 102,
    SELFTEST_AMPLIFIER_SAMPLE11 = 103,
    SELFTEST_AMPLIFIER_SAMPLE111 = 104,
    SELFTEST_AMPLIFIER_REFERENCE1 = 105,
    SELFTEST_AMPLIFIER_REFERENCE11 = 106,
    SELFTEST_AMPLIFIER_REFERENCE111 = 107,
    SELFTEST_AMPLIFIER_SETUPRESULT = 108,

    SELFTEST_LED230_ILED = 110,
    SELFTEST_LED230_DARKSAMPLE = 111,
    SELFTEST_LED230_DARKREFERENCE = 112,
    SELFTEST_LED230_SAMPLE = 113,
    SELFTEST_LED230_REFERENCE = 114,

    SELFTEST_LED260_ILED = 120,
    SELFTEST_LED260_DARKSAMPLE = 121,
    SELFTEST_LED260_DARKREFERENCE = 122,
    SELFTEST_LED260_SAMPLE = 123,
    SELFTEST_LED260_REFERENCE = 124,

    SELFTEST_LED280_ILED = 130,
    SELFTEST_LED280_DARKSAMPLE = 131,
    SELFTEST_LED280_DARKREFERENCE = 132,
    SELFTEST_LED280_SAMPLE = 133,
    SELFTEST_LED280_REFERENCE = 134,

    SELFTEST_LED340_ILED = 140,
    SELFTEST_LED340_DARKSAMPLE = 141,
    SELFTEST_LED340_DARKREFERENCE = 142,
    SELFTEST_LED340_SAMPLE = 143,
    SELFTEST_LED340_REFERENCE = 144,

    LEVELLING_LED230_SETUPRESULT = 150,
    LEVELLING_LED230_CURRENT = 151,
    LEVELLING_LED230_AMPLIFICATIONSAMPLE = 152,
    LEVELLING_LED230_AMPLIFICATIONREFERENCE = 153,

    LEVELLING_LED260_SETUPRESULT = 160,
    LEVELLING_LED260_CURRENT = 161,
    LEVELLING_LED260_AMPLIFICATIONSAMPLE = 162,
    LEVELLING_LED260_AMPLIFICATIONREFERENCE = 163,

    LEVELLING_LED280_SETUPRESULT = 170,
    LEVELLING_LED280_CURRENT = 171,
    LEVELLING_LED280_AMPLIFICATIONSAMPLE = 172,
    LEVELLING_LED280_AMPLIFICATIONREFERENCE = 173,

    LEVELLING_LED340_SETUPRESULT = 180,
    LEVELLING_LED340_CURRENT = 181,
    LEVELLING_LED340_AMPLIFICATIONSAMPLE = 182,
    LEVELLING_LED340_AMPLIFICATIONREFERENCE = 183
}

/// <summary>
/// Enumeration representing various error codes.
/// </summary>
public enum Error
{
    OK = 0,
    UNKNOWN_COMMAND = 1,
    INVALID_PARAMETER = 2,    
    SREC_FLASH_WRITE_ERROR = 4,
    SREC_UNSUPPORTED_TYPE = 5,
    SREC_INVALID_CRC = 6,
    SREC_INVALID_STRING = 7,
    LEVELLING_FAILED = 100,
}

/// <summary>
/// Enumeration representing different data types.
/// </summary>
public enum TypeOf
{
    STRING = 0,
    UINT32 = 1,
    DOUBLE = 2
}


/// <summary>
/// Flags enumeration for self-test components.
/// </summary>
[Flags]
public enum Selftest
{
    ILED_230 = 0x00000001,
    ILED_260 = 0x00000002,
    ILED_280 = 0x00000004,
    ILED_340 = 0x00000008,
    SAMPLE_230 = 0x00000010,
    SAMPLE_260 = 0x00000020,
    SAMPLE_280 = 0x00000040,
    SAMPLE_340 = 0x00000080,
    REFERENCE_230 = 0x00000100,
    REFERENCE_260 = 0x00000200,
    REFERENCE_280 = 0x00000400,
    REFERENCE_340 = 0x00000800,
    REFERENCE = 0x00001000,
    SAMPLE = 0x00002000,
    SPLITRATIO = 0x00004000,
    LEVELLING_230 = 0x00008000
}