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
    /// <summary>
    /// JSON key 'baseline'.
    /// </summary>
    public const string BASELINE = "baseline";

    /// <summary>
    /// JSON key 'air'.
    /// </summary>
    public const string AIR = "air";

    /// <summary>
    /// JSON key 'sample'.
    /// </summary>
    public const string SAMPLE = "sample";

    /// <summary>
    /// JSON key 'reference'.
    /// </summary>
    public const string REFERENCE = "reference";

    /// <summary>
    /// JSON key 'comment'.
    /// </summary>
    public const string COMMENT = "comment";

    /// <summary>
    /// JSON key '230'.
    /// </summary>
    public const string CH_230 = "230";

    /// <summary>
    /// JSON key '260'.
    /// </summary>
    public const string CH_260 = "260";

    /// <summary>
    /// JSON key '280'.
    /// </summary>
    public const string CH_280 = "280";

    /// <summary>
    /// JSON key '340'.
    /// </summary>
    public const string CH_340 = "340";

    /// <summary>
    /// JSON key 'dsDNA'.
    /// </summary>
    public const string dsDNA = "dsDNA";

    /// <summary>
    /// JSON key 'ssDNA'.
    /// </summary>
    public const string ssDNA = "ssDNA";

    /// <summary>
    /// JSON key 'ssRNA'.
    /// </summary>
    public const string ssRNA = "ssRNA";

    /// <summary>
    /// JSON key 'purity260/280'.
    /// </summary>
    public const string PURITY260_280 = "purity260/280";

    /// <summary>
    /// JSON key 'purity260/230'.
    /// </summary>
    public const string PURITY260_230 = "purity260/230";

    /// <summary>
    /// JSON key 'measurements'.
    /// </summary>
    public const string MEASUREMENTS = "measurements";

    /// <summary>
    /// JSON key 'values'.
    /// </summary>
    public const string VALUES = "values";

    /// <summary>
    /// JSON key 'results'.
    /// </summary>
    public const string RESULTS = "results";

    /// <summary>
    /// JSON key 'serialnumber'.
    /// </summary>
    public const string SERIALNUMBER = "serialnumber";

    /// <summary>
    /// JSON key 'firmwareVersion'.
    /// </summary>
    public const string FIRMWAREVERSION = "firmwareVersion";

    /// <summary>
    /// JSON key 'productionnumber'.
    /// </summary>
    public const string PRODUCTIONNUMBER = "productionnumber";

    /// <summary>
    /// JSON key 'levelling'.
    /// </summary>
    public const string LEVELLING = "levelling";

    /// <summary>
    /// JSON key 'result'.
    /// </summary>
    public const string LEVELLING_RESULT = "result";

    /// <summary>
    /// JSON key 'current'.
    /// </summary>
    public const string LEVELLING_CURRENT = "current";

    /// <summary>
    /// JSON key 'amplifiersample'.
    /// </summary>
    public const string LEVELLING_AMPLIFIERSAMPLE = "amplifiersample";

    /// <summary>
    /// JSON key 'amplifierreference'.
    /// </summary>
    public const string LEVELLING_AMPLIFIERREFERENCE = "amplifierreference";

    /// <summary>
    /// JSON key 'selftest'.
    /// </summary>
    public const string SELFTEST = "selftest";

    /// <summary>
    /// JSON key 'result'.
    /// </summary>
    public const string SELFTEST_RESULT = "result";

    /// <summary>
    /// JSON key 'splitration230nm'.
    /// </summary>
    public const string SELFTEST_SPLITRATION230NM = "splitration230nm";

    /// <summary>
    /// JSON key 'amplifiers'.
    /// </summary>
    public const string SELFTEST_AMPLIFIERS = "amplifiers";

    /// <summary>
    /// JSON key 'current'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_CURRENT = "current";

    /// <summary>
    /// JSON key 'sample1'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_SAMPLE1 = "sample1";

    /// <summary>
    /// JSON key 'sample11'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_SAMPLE11 = "sample11";

    /// <summary>
    /// JSON key 'sample111'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_SAMPLE111 = "sample111";

    /// <summary>
    /// JSON key 'reference1'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_REFERENCE1 = "reference1";

    /// <summary>
    /// JSON key 'reference11'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_REFERENCE11 = "reference11";

    /// <summary>
    /// JSON key 'reference111'.
    /// </summary>
    public const string SELFTEST_AMPLIFIER_REFERENCE111 = "reference111";

    /// <summary>
    /// JSON key 'iled'.
    /// </summary>
    public const string SELFTEST_ILED = "iled";

    /// <summary>
    /// JSON key 'darksample'.
    /// </summary>
    public const string SELFTEST_DARKSAMPLE = "darksample";

    /// <summary>
    /// JSON key 'darkreference'.
    /// </summary>
    public const string SELFTEST_DARKREFERENCE = "darkreference";

    /// <summary>
    /// JSON key 'sample'.
    /// </summary>
    public const string SELFTEST_SAMPLE = "sample";

    /// <summary>
    /// JSON key 'reference'.
    /// </summary>
    public const string SELFTEST_REFERENCE = "reference";
    /// <summary>
    /// JSON key 'logging'.
    /// </summary>
    public const string LOGGING = "logging";

    /// <summary>
    /// JSON key 'data_time'.
    /// </summary>
    public const string DATE_TIME = "date_time";

    /// <summary>
    /// JSON key 'errors'.
    /// </summary>
    public const string ERRORS = "errors";
}

/// <summary>
/// Enumeration representing different index values for hardware configurations.
/// </summary>
public enum Index
{
    /// <summary>
    /// Index for version.
    /// </summary>
    VERSION = 0,

    /// <summary>
    /// Index for serial number.
    /// </summary>
    SERIALNUMBER = 1,

    /// <summary>
    /// Index for production number.
    /// </summary>
    PRODUCTIONNUMBER = 3,

    /// <summary>
    /// Index for last measurement count.
    /// </summary>
    LAST_MEASUREMENT_COUNT = 10,

    /// <summary>
    /// Index for max 230 nm led current.
    /// </summary>
    LED230NM_MAX_CURRENT = 23,

    /// <summary>
    /// Index for max 260 nm led current.
    /// </summary>
    LED260NM_MAX_CURRENT = 33,

    /// <summary>
    /// Index for max 280 nm led current.
    /// </summary>
    LED280NM_MAX_CURRENT = 43,

    /// <summary>
    /// Index for max 340 nm led current.
    /// </summary>
    LED340NM_MAX_CURRENT = 53,

    /// <summary>
    /// Index for amplifier split ratio @ 230 nm.
    /// </summary>

    SELFTEST_AMPLIFIER_SPLITRATIO230NM = 100,

    /// <summary>
    /// Index for amplfier selft test current.
    /// </summary>
    SELFTEST_AMPLIFIER_CURRENT = 101,

    /// <summary>
    /// Index for amplifier self-test sample value (amplification = 1.1).
    /// </summary>
    SELFTEST_AMPLIFIER_SAMPLE1 = 102,

    /// <summary>
    /// Index for amplifier self-test sample value (amplification = 11.0).
    /// </summary>
    SELFTEST_AMPLIFIER_SAMPLE11 = 103,

    /// <summary>
    /// Index for amplifier self-test sample value (amplification = 111.0).
    /// </summary>
    SELFTEST_AMPLIFIER_SAMPLE111 = 104,

    /// <summary>
    /// Index for amplifier self-test reference value (amplification = 1.1).
    /// </summary>
    SELFTEST_AMPLIFIER_REFERENCE1 = 105,

    /// <summary>
    /// Index for amplifier self-test reference value (amplification = 11.0).
    /// </summary>
    SELFTEST_AMPLIFIER_REFERENCE11 = 106,

    /// <summary>
    /// Index for amplifier self-test reference value (amplification = 111.0).
    /// </summary>
    SELFTEST_AMPLIFIER_REFERENCE111 = 107,

    /// <summary>
    /// Index for amplifier self-test result.
    /// </summary>
    SELFTEST_AMPLIFIER_SETUPRESULT = 108,

    /// <summary>
    /// Index for 230 nm led self-test current.
    /// </summary>
    SELFTEST_LED230_ILED = 110,

    /// <summary>
    /// Index for 230 nm led self-test dark sample value.
    /// </summary>
    SELFTEST_LED230_DARKSAMPLE = 111,

    /// <summary>
    /// Index for 230 nm led self-test dark reference value.
    /// </summary>
    SELFTEST_LED230_DARKREFERENCE = 112,

    /// <summary>
    /// Index for 230 nm led self-test sample value.
    /// </summary>
    SELFTEST_LED230_SAMPLE = 113,

    /// <summary>
    /// Index for 230 nm led self-test reference value.
    /// </summary>
    SELFTEST_LED230_REFERENCE = 114,

    /// <summary>
    /// Index for 260 nm led self-test current.
    /// </summary>
    SELFTEST_LED260_ILED = 120,

    /// <summary>
    /// Index for 260 nm led self-test dark sample value.
    /// </summary>
    SELFTEST_LED260_DARKSAMPLE = 121,

    /// <summary>
    /// Index for 260 nm led self-test dark reference value.
    /// </summary>
    SELFTEST_LED260_DARKREFERENCE = 122,

    /// <summary>
    /// Index for 260 nm led self-test sample value.
    /// </summary>
    SELFTEST_LED260_SAMPLE = 123,

    /// <summary>
    /// Index for 260 nm led self-test reference value.
    /// </summary>
    SELFTEST_LED260_REFERENCE = 124,

    /// <summary>
    /// Index for 280 nm led self-test current.
    /// </summary>
    SELFTEST_LED280_ILED = 130,

    /// <summary>
    /// Index for 280 nm led self-test dark sample value.
    /// </summary>
    SELFTEST_LED280_DARKSAMPLE = 131,

    /// <summary>
    /// Index for 280 nm led self-test dark reference value.
    /// </summary>
    SELFTEST_LED280_DARKREFERENCE = 132,

    /// <summary>
    /// Index for 280 nm led self-test sample value.
    /// </summary>
    SELFTEST_LED280_SAMPLE = 133,

    /// <summary>
    /// Index for 280 nm led self-test reference value.
    /// </summary>
    SELFTEST_LED280_REFERENCE = 134,

    /// <summary>
    /// Index for 340 nm led self-test current.
    /// </summary>
    SELFTEST_LED340_ILED = 140,

    /// <summary>
    /// Index for 340 nm led self-test dark sample value.
    /// </summary>
    SELFTEST_LED340_DARKSAMPLE = 141,

    /// <summary>
    /// Index for 340 nm led self-test dark reference value.
    /// </summary>
    SELFTEST_LED340_DARKREFERENCE = 142,

    /// <summary>
    /// Index for 340 nm led self-test sample value.
    /// </summary>
    SELFTEST_LED340_SAMPLE = 143,

    /// <summary>
    /// Index for 340 nm led self-test reference value.
    /// </summary>
    SELFTEST_LED340_REFERENCE = 144,

    /// <summary>
    /// Index for 230 nm self-test result.
    /// </summary>
    LEVELLING_LED230_SETUPRESULT = 150,

    /// <summary>
    /// Index for 230 nm levelling current.
    /// </summary>
    LEVELLING_LED230_CURRENT = 151,

    /// <summary>
    /// Index for 230 nm amplification sample channel.
    /// </summary>
    LEVELLING_LED230_AMPLIFICATIONSAMPLE = 152,

    /// <summary>
    /// Index for 230 nm amplification reference channel.
    /// </summary>
    LEVELLING_LED230_AMPLIFICATIONREFERENCE = 153,

    /// <summary>
    /// Index for 260 nm self-test result.
    /// </summary>
    LEVELLING_LED260_SETUPRESULT = 160,

    /// <summary>
    /// Index for 260 nm levelling current.
    /// </summary>
    LEVELLING_LED260_CURRENT = 161,

    /// <summary>
    /// Index for 260 nm amplification sample channel.
    /// </summary>
    LEVELLING_LED260_AMPLIFICATIONSAMPLE = 162,

    /// <summary>
    /// Index for 260 nm amplification reference channel.
    /// </summary>
    LEVELLING_LED260_AMPLIFICATIONREFERENCE = 163,

    /// <summary>
    /// Index for 280 nm self-test result.
    /// </summary>
    LEVELLING_LED280_SETUPRESULT = 170,

    /// <summary>
    /// Index for 280 nm levelling current.
    /// </summary>
    LEVELLING_LED280_CURRENT = 171,

    /// <summary>
    /// Index for 280 nm amplification sample channel.
    /// </summary>
    LEVELLING_LED280_AMPLIFICATIONSAMPLE = 172,

    /// <summary>
    /// Index for 280 nm amplification reference channel.
    /// </summary>
    LEVELLING_LED280_AMPLIFICATIONREFERENCE = 173,

    /// <summary>
    /// Index for 340 nm self-test result.
    /// </summary>
    LEVELLING_LED340_SETUPRESULT = 180,

    /// <summary>
    /// Index for 340 nm levelling current.
    /// </summary>
    LEVELLING_LED340_CURRENT = 181,

    /// <summary>
    /// Index for 340 nm amplification sample channel.
    /// </summary>
    LEVELLING_LED340_AMPLIFICATIONSAMPLE = 182,

    /// <summary>
    /// Index for 340 nm amplification reference channel.
    /// </summary>
    LEVELLING_LED340_AMPLIFICATIONREFERENCE = 183
}

/// <summary>
/// Enumeration representing various error codes.
/// </summary>
public enum Error
{
    /// <summary>
    /// No error.
    /// </summary>
    OK = 0,

    /// <summary>
    /// Unknown command.
    /// </summary>
    UNKNOWN_COMMAND = 1,

    /// <summary>
    /// Invalid parameter.
    /// </summary>
    INVALID_PARAMETER = 2,

    /// <summary>
    /// Flash write error.
    /// </summary>
    SREC_FLASH_WRITE_ERROR = 4,

    /// <summary>
    /// Unsupported SREC type.
    /// </summary>
    SREC_UNSUPPORTED_TYPE = 5,

    /// <summary>
    /// Invalid SREC crc.
    /// </summary>
    SREC_INVALID_CRC = 6,

    /// <summary>
    /// Invalid SREC string.
    /// </summary>
    SREC_INVALID_STRING = 7,

    /// <summary>
    /// Levelling failed.
    /// </summary>
    LEVELLING_FAILED = 100,
}

/// <summary>
/// Enumeration representing different data types.
/// </summary>
public enum TypeOf
{
    /// <summary>
    /// String type
    /// </summary>
    STRING = 0,

    /// <summary>
    /// Uint32 type
    /// </summary>
    UINT32 = 1,

    /// <summary>
    /// Double type
    /// </summary>
    DOUBLE = 2
}


/// <summary>
/// Flags enumeration for self-test components.
/// </summary>
[Flags]
public enum Selftest
{
    /// <summary>
    /// Indicates that 230 nm led current is not in the expected range.
    /// </summary>
    ILED_230 = 0x00000001,

    /// <summary>
    /// Indicates that 260 nm led current is not in the expected range.
    /// </summary>
    ILED_260 = 0x00000002,

    /// <summary>
    /// Indicates that 280 nm led current is not in the expected range.
    /// </summary>
    ILED_280 = 0x00000004,

    /// <summary>
    /// Indicates that 340 nm led current is not in the expected range.
    /// </summary>
    ILED_340 = 0x00000008,

    /// <summary>
    /// Indicates that 230 nm led sample value is not in the expected range.
    /// </summary>
    SAMPLE_230 = 0x00000010,

    /// <summary>
    /// Indicates that 260 nm led sample value is not in the expected range.
    /// </summary>
    SAMPLE_260 = 0x00000020,

    /// <summary>
    /// Indicates that 280 nm led sample value is not in the expected range.
    /// </summary>
    SAMPLE_280 = 0x00000040,

    /// <summary>
    /// Indicates that 340 nm led sample value is not in the expected range.
    /// </summary>
    SAMPLE_340 = 0x00000080,

    /// <summary>
    /// Indicates that 230 nm led reference value is not in the expected range.
    /// </summary>
    REFERENCE_230 = 0x00000100,

    /// <summary>
    /// Indicates that 260 nm led reference value is not in the expected range.
    /// </summary>
    REFERENCE_260 = 0x00000200,

    /// <summary>
    /// Indicates that 280 nm led reference value is not in the expected range.
    /// </summary>
    REFERENCE_280 = 0x00000400,

    /// <summary>
    /// Indicates that 340 nm led reference value is not in the expected range.
    /// </summary>
    REFERENCE_340 = 0x00000800,

    /// <summary>
    /// Indicates that reference value is not in the expected range.
    /// </summary>
    REFERENCE = 0x00001000,

    /// <summary>
    /// Indicates that sample value is not in the expected range.
    /// </summary>
    SAMPLE = 0x00002000,

    /// <summary>
    /// Indicates that split ratio is not in the expected range.
    /// </summary>
    SPLITRATIO = 0x00004000,

    /// <summary>
    /// Indicates that the levelling for the 230 nm led failed.
    /// </summary>
    LEVELLING_230 = 0x00008000
}

/// <summary>
/// Enumeration representing different status led colors .
/// </summary>
public enum StatusLedColor
{
    /// <summary>
    /// Status LED off.
    /// </summary>
    OFF = 0,

    /// <summary>
    /// Status LED red.
    /// </summary>
    RED = 1,

    /// <summary>
    /// Status LED green.
    /// </summary>
    GREEN = 2,

    /// <summary>
    /// Status LED blue.
    /// </summary>
    BLUE = 3,
}

/// <summary>
/// Enumeration representing different led wave lengths.
/// </summary>
public enum Led
{
    /// <summary>
    /// All leds off.
    /// </summary>
    LED_OFF = 0,

    /// <summary>
    /// 230 nm led on.
    /// </summary>
    LED_230 = 1,

    /// <summary>
    /// 260 nm led on.
    /// </summary>
    LED_260 = 2,

    /// <summary>
    /// 280 nm led on.
    /// </summary>
    LED_280 = 3,

    /// <summary>
    /// 340 nm led on.
    /// </summary>
    LED_340 = 4
}

/// <summary>
/// Enumeration representing the two amplifiers.
/// </summary>
public enum Amplifier
{
    /// <summary>
    /// Sample amplifier.
    /// </summary>
    SAMPLE = 0,

    /// <summary>
    /// Reference amplifier.
    /// </summary>
    REFERENCE = 1
}

/// <summary>
/// Enumeration representing amplification factor of the amplifiers.
/// </summary>
public enum AmplifierFactors
{
    /// <summary>
    /// Amplification factor 1.1.
    /// </summary>
    F_1_1 = 0,

    /// <summary>
    /// Amplification factor 11.0.
    /// </summary>
    F_11_0 = 1,

    /// <summary>
    /// Amplification factor 111.0.
    /// </summary>
    F_111_0 = 2,
}