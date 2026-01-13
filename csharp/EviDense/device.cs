// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using Hse.EviDense;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Management;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json.Nodes;
using System.Text.RegularExpressions;

namespace Hse.EviDense;

/// <summary>
/// Represents the ADC values.
/// </summary>
public class AdcResult
{
    /// <summary>
    /// Voltage of sample detector in uV.
    /// </summary>
    public int SampleValue { get; set; }

    /// <summary>
    /// Voltage of reference detector in uV.
    /// </summary>
    public int ReferenceValue { get; set; }

    /// <summary>
    /// Voltage of internal 5V reference in uV.
    /// </summary>
    public int Vref { get; set; }

    /// <summary>
    /// LED current in uA.
    /// </summary>
    public int Current { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="AdcResult"/> class.
    /// </summary>
    /// <param name="sampleValue">Voltage of sample detector in uV.</param>
    /// <param name="referenceValue">Voltage of reference detector in uV.</param>
    /// <param name="vref">Voltage of internal 5V reference in uV.</param>
    /// <param name="current">LED current in uA.</param>
    public AdcResult(int sampleValue, int referenceValue, int vref, int current)
    {
        SampleValue = sampleValue;
        ReferenceValue = referenceValue;
        Vref = vref;
        Current = current;
    }
}

/// <summary>
/// Represents the result of a levelling operation for a single measurement channel.
/// </summary>
public class LevellingChannelResult
{
    /// <summary>
    /// Gets or sets the overall result of the levelling operation.
    /// </summary>
    public int Result { get; set; }

    /// <summary>
    /// Gets or sets the current in uA applied during the levelling process.
    /// </summary>
    public int Current { get; set; }

    /// <summary>
    /// Gets or sets the amplification value for the sample measurement.
    /// </summary>
    /// <remarks>
    /// 0: factor 1.1
    /// 1: factor 11.0
    /// 2: factor 111.0
    /// </remarks>
    public int AmplificationSample { get; set; }

    /// <summary>
    /// Gets or sets the amplification value for the reference measurement.
    /// </summary>
    /// <remarks>
    /// 0: factor 1.1
    /// 1: factor 11.0
    /// 2: factor 111.0
    /// </remarks>
    public int AmplificationReference { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="LevellingChannelResult"/> class.
    /// </summary>
    /// <param name="result">The overall levelling result.</param>
    /// <param name="current">The applied current in uA during levelling.</param>
    /// <param name="amplificationSample">The amplification value for the sample.</param>
    /// <param name="amplificationReference">The amplification value for the reference.</param>
    public LevellingChannelResult(int result, int current, int amplificationSample, int amplificationReference)
    {
        Result = result;
        Current = current;
        AmplificationSample = amplificationSample;
        AmplificationReference = amplificationReference;
    }

    /// <summary>
    /// Returns a string representation of the levelling result.
    /// </summary>
    /// <returns>A formatted string with levelling parameters.</returns>
    public override string ToString()
    {
        return $"result:{Result} current:{Current} amplificationSample:{AmplificationSample} amplificationReference:{AmplificationReference}";
    }

    /// <summary>
    /// Converts the levelling result into a JSON representation.
    /// </summary>
    /// <returns>A <see cref="JsonNode"/> containing levelling result data.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.LEVELLING_RESULT] = Result;
        obj[Dict.LEVELLING_CURRENT] = Current;
        obj[Dict.LEVELLING_AMPLIFIERSAMPLE] = AmplificationSample;
        obj[Dict.LEVELLING_AMPLIFIERREFERENCE] = AmplificationReference;

        return obj;
    }
}

/// <summary>
/// Represents the overall levelling results for multiple measurement channels.
/// </summary>
public class LevellingResult
{
    /// <summary>
    /// Gets or sets the levelling result for the 230nm measurement channel.
    /// </summary>
    public LevellingChannelResult Channel230 { get; set; }

    /// <summary>
    /// Gets or sets the levelling result for the 260nm measurement channel.
    /// </summary>
    public LevellingChannelResult Channel260 { get; set; }

    /// <summary>
    /// Gets or sets the levelling result for the 280nm measurement channel.
    /// </summary>
    public LevellingChannelResult Channel280 { get; set; }

    /// <summary>
    /// Gets or sets the levelling result for the 340nm measurement channel.
    /// </summary>
    public LevellingChannelResult Channel340 { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="LevellingResult"/> class.
    /// </summary>
    /// <param name="channel230">Levelling result for the 230nm channel.</param>
    /// <param name="channel260">Levelling result for the 260nm channel.</param>
    /// <param name="channel280">Levelling result for the 280nm channel.</param>
    /// <param name="channel340">Levelling result for the 340nm channel.</param>
    public LevellingResult(LevellingChannelResult channel230, LevellingChannelResult channel260, LevellingChannelResult channel280, LevellingChannelResult channel340)
    {
        Channel230 = channel230;
        Channel260 = channel260;
        Channel280 = channel280;
        Channel340 = channel340;
    }

    /// <summary>
    /// Returns a string representation of the levelling results for all channels.
    /// </summary>
    /// <returns>A formatted string containing levelling results for all channels.</returns>
    public override string ToString()
    {
        return $"230:{Channel230} 260:{Channel260} 280:{Channel280} 340:{Channel340}";
    }

    /// <summary>
    /// Converts the levelling results into a JSON representation.
    /// </summary>
    /// <returns>A <see cref="JsonNode"/> containing levelling results for all channels.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.CH_230] = Channel230.ToJson();
        obj[Dict.CH_260] = Channel260.ToJson();
        obj[Dict.CH_280] = Channel280.ToJson();
        obj[Dict.CH_340] = Channel340.ToJson();

        return obj;
    }
}

/// <summary>
/// Represents the result of a self-test, containing various checks and problem indicators.
/// </summary>
public class SelfTestResult
{
    /// <summary>
    /// Gets or sets the integer result of the self-test.
    /// </summary>
    public int Result { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="SelfTestResult"/> class.
    /// </summary>
    /// <param name="result">The result of the self-test.</param>
    public SelfTestResult(int result)
    {
        Result = result;
    }

    /// <summary>
    /// Determines whether there are any problems in the self-test.
    /// </summary>
    /// <returns>True if there are problems; otherwise, false.</returns>
    public bool HasProblems()
    {
        return Result != 0;
    }

    /// <summary>
    /// Checks if there is a problem with the current flow of the 230nm LED.
    /// </summary>
    public bool HasProblemWithILed230() => (Result & (int)Selftest.ILED_230) != 0;

    /// <summary>
    /// Checks if there is a problem with the current flow of the 260nm LED.
    /// </summary>
    public bool HasProblemWithILed260() => (Result & (int)Selftest.ILED_260) != 0;

    /// <summary>
    /// Checks if there is a problem with the current flow of the 280nm LED.
    /// </summary>
    public bool HasProblemWithILed280() => (Result & (int)Selftest.ILED_280) != 0;

    /// <summary>
    /// Checks if there is a problem with the current flow of the 340nm LED.
    /// </summary>
    public bool HasProblemWithILed340() => (Result & (int)Selftest.ILED_340) != 0;

    /// <summary>
    /// Checks whether there is a problem with the sample detector when the 230nm LED lights up.
    /// </summary>
    public bool HasProblemWithSample230() => (Result & (int)Selftest.SAMPLE_230) != 0;

    /// <summary>
    /// Checks whether there is a problem with the sample detector when the 260nm LED lights up.
    /// </summary>
    public bool HasProblemWithSample260() => (Result & (int)Selftest.SAMPLE_260) != 0;

    /// <summary>
    /// Checks whether there is a problem with the sample detector when the 280nm LED lights up.
    /// </summary>
    public bool HasProblemWithSample280() => (Result & (int)Selftest.SAMPLE_280) != 0;

    /// <summary>
    /// Checks whether there is a problem with the sample detector when the 340nm LED lights up.
    /// </summary>
    public bool HasProblemWithSample340() => (Result & (int)Selftest.SAMPLE_340) != 0;

    /// <summary>
    /// Checks whether there is a problem with the reference detector when the 230nm LED lights up.
    /// </summary>
    public bool HasProblemWithReference230() => (Result & (int)Selftest.REFERENCE_230) != 0;

    /// <summary>
    /// Checks whether there is a problem with the reference detector when the 260nm LED lights up.
    /// </summary>
    public bool HasProblemWithReference260() => (Result & (int)Selftest.REFERENCE_260) != 0;

    /// <summary>
    /// Checks whether there is a problem with the reference detector when the 280nm LED lights up.
    /// </summary>
    public bool HasProblemWithReference280() => (Result & (int)Selftest.REFERENCE_280) != 0;

    /// <summary>
    /// Checks whether there is a problem with the reference detector when the 340nm LED lights up.
    /// </summary>
    public bool HasProblemWithReference340() => (Result & (int)Selftest.REFERENCE_340) != 0;

    /// <summary>
    /// Checks if there is a problem with the reference detector.
    /// </summary>
    public bool HasProblemWithReferencDetector() => (Result & (int)Selftest.REFERENCE) != 0;

    /// <summary>
    /// Checks if there is a problem with the sample detector.
    /// </summary>
    public bool HasProblemWithSampleDetector() => (Result & (int)Selftest.SAMPLE) != 0;

    /// <summary>
    /// Checks if there is a problem with the split ratio between sample and reference detector.
    /// </summary>
    public bool HasProblemWithSplitration() => (Result & (int)Selftest.SPLITRATIO) != 0;

    /// <summary>
    /// Checks if there is a problem with levelling at 230 nm.
    /// </summary>
    public bool HasProblemWithLevelling230() => (Result & (int)Selftest.LEVELLING_230) != 0;

    /// <summary>
    /// Converts the self-test result to a JSON representation.
    /// </summary>
    /// <returns>A JsonNode representing the self-test result.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();
        obj[Dict.SELFTEST_RESULT] = Result;
        return obj;
    }
}

/// <summary>
/// This class represents the eviDense module.
/// </summary>
public class Device : IDisposable
{
    private SerialPort ? serialPort_;
    private string serialNumber_ = "?";
    private string firmwareVersion_ = "?";
    private bool isSimulation_;

    /// <summary>
    /// Releases all resources used by the <see cref="Device"/>.
    /// </summary>
    public void Dispose()
    {
        if (serialPort_ != null && serialPort_.IsOpen)
            serialPort_.Close();

        if (serialPort_ != null)
            serialPort_.Dispose();
    }

    /// <summary>
    /// Initializes a new instance of the <see cref="Device"/> class.
    /// The constructor attempts to connect to an EviDense module via a serial port.
    /// If no serial port is provided, it tries to auto-detect the device.
    ///
    /// Throws an exception if:
    /// - No device is found during auto-detection.
    /// - The provided serial port does not match the expected VID/PID for the EviDense device.
    /// </summary>
    /// <param name="serialNumber">
    /// The serial number of the device to connect to. If null or empty, the constructor
    /// attempts to automatically find a device.
    /// </param>
    /// <exception cref="Exception">
    /// Thrown when:
    /// - No device is found during auto-detection (if <paramref name="serialNumber"/> is null or empty).
    /// - The provided serial port does not correspond to an EviDense device.
    /// </exception>
    public Device(string? serialNumber = null)
    {
        if ((serialNumber != null) && (serialNumber == "SIMULATION"))
        {
            isSimulation_ = true;
        }
        else
        {
            isSimulation_ = false;

            var serialPortName = DeviceFinder.FindDevice(serialNumber);
            if (serialPortName == null)
            {
                throw new Exception($"EviDense ({serialNumber}) module not found");
            }

            serialPort_ = new SerialPort(serialPortName, 115200, Parity.None, 8, StopBits.One)
            {
                ReadTimeout = 30000
            };
            serialPort_.Open();
            serialPort_.DiscardInBuffer();
            serialPort_.DiscardOutBuffer();

            try
            {
                SerialNumber();
                FirmwareVersion();
            }
            catch (Exception)
            {
                // silent fail
            }
        }
    }

    /// <summary>
    /// Finalizer for <see cref="Device"/>.
    /// Ensures the device is properly disposed.
    /// </summary>
    ~Device()
    {
        Dispose();
    }

    /// <summary>
    /// Gets the version of the eviDense library.
    /// </summary>
    public string LibraryVersion
    {
        get
        {
            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            System.Diagnostics.FileVersionInfo fvi = System.Diagnostics.FileVersionInfo.GetVersionInfo(assembly.Location);
            return fvi.FileVersion ?? string.Empty;
        }
    }

    /// <summary>
    /// Retrieves a list of available eviDense devices by scanning serial ports.    
    /// </summary>
    /// <returns>A list of serial numbers of available matching devices.</returns>
    public static List<string> GetAvailableDevices()
    {
        return DeviceFinder.GetAvailableDevices();
    }

    /// <summary>
    /// Splits a device response into tokens, preserving quoted substrings.
    /// </summary>
    /// <param name="input">Raw line as returned by the device (without leading ':').</param>
    /// <returns>Array of tokens; quoted segments are returned without quotes.</returns>
    private static string[] Split(string input)
    {
        var pattern = @"(?:""(?<q>[^""]*)"")|(?:'(?<q>[^']*)')|(?<q>\S+)";
        var matches = Regex.Matches(input, pattern);

        string[] tokens = new string[matches.Count];
        for (int i = 0; i < matches.Count; i++)
        {
            tokens[i] = matches[i].Groups["q"].Value;
        }

        return tokens;
    }

    /// <summary>
    /// Sends a command string to the device over the serial interface and waits for a valid response.
    /// </summary>
    /// <param name="tx">The command string to transmit (without prefix or newline, e.g., "V 1").</param>
    /// <returns>
    /// A list of response string tokens received from the device after processing the command. 
    /// Each token represents a space- or quote-delimited component of the response.
    /// </returns>
    /// <exception cref="Exception">
    /// Thrown in the following cases:
    /// <list type="bullet">
    /// <item><description>No response within the timeout period.</description></item>
    /// <item><description>Response does not start with ':' (invalid format).</description></item>
    /// <item><description>Response indicates an error code (e.g., unknown command, invalid parameter).</description></item>
    /// </list>
    /// </exception>
    /// <remarks>
    /// This method prepends a ':' to the transmitted command, sends it via the serial port,
    /// then reads and validates the response. If an error is indicated by the device (response starting with "E"),
    /// a corresponding error message is thrown based on the error code.
    /// </remarks>
    public string[] Command(string tx)
    {
        tx = $":{tx}";
        var rx = "";
        if (isSimulation_)
        {
            rx = XferSocket(tx);
        }
        else
        {
            rx = XferSerial(tx);
        }

        if (string.IsNullOrEmpty(rx))
            throw new Exception("No response within time!");

        if (!rx.StartsWith(":"))
            throw new Exception($"Response did not start with ':' {rx}");

        string[] parts = Split(rx.Remove(0, 1));

        if (parts[0] == "E")
        {
            int errorCode = -1;

            if (parts.Length >= 2)
                if (int.TryParse(parts[1], out errorCode))
                    throw new Exception($"Response of TX:{tx} has an error: {ErrorToText(errorCode)}");
                else
                    throw new Exception($"Response of TX:{tx} has an unknown error: {rx}");
        }
        else
        {
            if (tx.Remove(0, 1).Split()[0] != parts[0])
            {
                throw new Exception($"Response for sent command '{tx}' does not start with same command: '{rx}'");
            }
        }

        return parts;
    }

    /// <summary>
    /// Transmits a single command over TCP loopback to a simulator and returns the first response line.
    /// </summary>
    /// <param name="tx">Command with leading ':' (as prepared by <see cref="Command(string)"/>).</param>
    /// <returns>The raw response line, or <c>null</c> if the connection is closed.</returns>
    private string? XferSocket(string tx)
    {
        using var client = new TcpClient();
        client.Connect("127.0.0.1", 5000);

        using NetworkStream stream = client.GetStream();
        using var reader = new StreamReader(stream, Encoding.UTF8);
        using var writer = new StreamWriter(stream, Encoding.UTF8) { AutoFlush = true };

        var outBytes = Encoding.UTF8.GetBytes(":" + tx + "\n");
        writer.WriteLineAsync(tx);

        var ret = reader.ReadLine();

        client.Close();

        return ret;
    }

    /// <summary>
    /// Transmits a single command over the configured serial port and reads the response line.
    /// </summary>
    /// <param name="tx">Command with leading ':' (as prepared by <see cref="Command(string)"/>).</param>
    /// <returns>The raw response line from the device.</returns>
    /// <exception cref="InvalidOperationException">If the serial port is not initialized.</exception>
    private string? XferSerial(string tx)
    {
        if (serialPort_ == null)
        {
            throw new Exception("SerialPort cant be null!");
        }
        serialPort_.DiscardInBuffer();
        serialPort_.DiscardOutBuffer();

        serialPort_.WriteLine(tx);

        return serialPort_.ReadLine();
    }

    private string ErrorToText(int error)
    {
        switch (error)
        {
            case (int)Error.OK:
                return "OK";

            case (int)Error.UNKNOWN_COMMAND:
                return "Unknown command";

            case (int)Error.INVALID_PARAMETER:
                return "Invalid parameter";

            case (int)Error.SREC_FLASH_WRITE_ERROR:
                return "Invalid parameter";

            case (int)Error.SREC_UNSUPPORTED_TYPE:
                return "SREC: unsupported type";

            case (int)Error.SREC_INVALID_CRC:
                return "SREC: invalid crc";

            case (int)Error.SREC_INVALID_STRING:
                return "SREC: invalid string";

            case (int)Error.LEVELLING_FAILED:
                return "Levelling failed";

            default:
                return "Unknown error";
        }
    }

    /// <summary>
    /// Retrieves a value from the device at the specified index and converts it to the desired type.
    /// Sends a command to the device to fetch the value and parses the response.
    /// </summary>
    /// <typeparam name="T">
    /// The type to which the retrieved value will be converted. Must be compatible with <see cref="Convert.ChangeType(object, Type)"/>.
    /// </typeparam>
    /// <param name="index">
    /// The index from which the value should be retrieved.
    /// </param>
    /// <returns>
    /// The value retrieved from the device, converted to the specified type <typeparamref name="T"/>.
    /// </returns>
    /// <exception cref="InvalidCastException">
    /// Thrown if the conversion to the specified type <typeparamref name="T"/> fails.
    /// </exception>
    /// <exception cref="Exception">
    /// Thrown if the device response is invalid or does not contain the expected data format.
    /// </exception>
    public T Get<T>(Index index)
    {
        string[] response = Command($"V {(int)index}");
        return (T)Convert.ChangeType(response[1], typeof(T));
    }

    /// <summary>
    /// Sets a value on the device at the specified index.
    /// Sends a command to update the value on the device.
    /// </summary>
    /// <param name="index">The index where the value should be set.</param>
    /// <param name="value">The integer value to set at the specified index.</param>
    public void Set(Index index, int value)
    {
        string[] response = Command($"V {(int)index} {value}");
    }

    /// <summary>
    /// Sets a value on the device at the specified index.
    /// Sends a command to update the value on the device.
    /// </summary>
    /// <param name="index">The index where the value should be set.</param>
    /// <param name="value">The double value to set at the specified index.</param>
    public void Set(Index index, double value)
    {
        string[] response = Command($"V {(int)index} {value}");
    }

    /// <summary>
    /// Sets a value on the device at the specified index.
    /// Sends a command to update the value on the device.
    /// </summary>
    /// <param name="index">The index where the value should be set.</param>
    /// <param name="value">The string value to set at the specified index.</param>
    public void Set(Index index, string value)
    {
        string[] response = Command($"V {(int)index} {value}");
    }

    /// <summary>
    /// Retrieves the serial number of the connected device.
    /// This method uses the <see cref="Get{T}(Index)"/> method to fetch the serial number
    /// from the device at the predefined index for serial numbers.
    /// </summary>
    /// <returns>
    /// The serial number of the device as a string.
    /// </returns>
    /// <exception cref="InvalidCastException">
    /// Thrown if the retrieval or conversion of the serial number fails.
    /// </exception>
    /// <exception cref="Exception">
    /// Thrown if there is an issue communicating with the device or retrieving the value.
    /// </exception>
    public string SerialNumber()
    {
        var serialNumber = Get<string>(Index.SERIALNUMBER);

        // save the serial number for the ToString() function
        serialNumber_ = serialNumber;
        return serialNumber;
    }


    /// <summary>
    /// Retrieves the firmware version of the connected device.
    /// This method uses the <see cref="Get{T}(Index)"/> method to fetch the firmware version
    /// from the device at the predefined index for firmware versions.
    /// </summary>
    /// <returns>
    /// The firmware version of the device as a string.
    /// </returns>
    /// <exception cref="InvalidCastException">
    /// Thrown if the retrieval or conversion of the firmware version fails.
    /// </exception>
    /// <exception cref="Exception">
    /// Thrown if there is an issue communicating with the device or retrieving the value.
    /// </exception>
    public string FirmwareVersion()
    {
        var firmwareVersion = Get<string>(Index.VERSION);

        // save the firmware version for the ToString() function
        serialNumber_ = firmwareVersion;
        return firmwareVersion;
    }


    /// <summary>
    /// Retrieves the production number of the connected device.
    /// This method uses the <see cref="Get{T}(Index)"/> method to fetch the production number
    /// from the device at the predefined index.
    /// </summary>
    /// <returns>
    /// The production number of the device as a string.
    /// </returns>
    /// <exception cref="Exception">
    /// Thrown if there is an issue communicating with the device or retrieving the value.
    /// </exception>
    public string ProductionNumber()
    {
        return Get<string>(Index.PRODUCTIONNUMBER);
    }

    /// <summary>
    /// Retrieves levelling data from the device and parses it into a <see cref="LevellingResult"/> object.
    /// </summary>
    /// <returns>
    /// A <see cref="LevellingResult"/> object containing the parsed levelling data for all channels.
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if any of the device's response values cannot be parsed as integers.
    /// </exception>
    /// <exception cref="Exception">
    /// Thrown if the device response is invalid or does not contain the expected number of values.
    /// </exception>
    /// <remarks>
    /// This command expects and empty cuvette guide.
    /// Each LED channel current is set so that the detector measures 90% of the maximum level.
    /// </remarks>
    public LevellingResult Levelling()
    {
        string[] response = Command("C");
        return new LevellingResult(
            new LevellingChannelResult(int.Parse(response[1]), int.Parse(response[2]), int.Parse(response[3]), int.Parse(response[4])),
            new LevellingChannelResult(int.Parse(response[5]), int.Parse(response[6]), int.Parse(response[7]), int.Parse(response[8])),
            new LevellingChannelResult(int.Parse(response[9]), int.Parse(response[10]), int.Parse(response[11]), int.Parse(response[12])),
            new LevellingChannelResult(int.Parse(response[13]), int.Parse(response[14]), int.Parse(response[15]), int.Parse(response[16]))
        );
    }

    /// <summary>
    /// Retrieves the baseline measurement data from the device and constructs a <see cref="SingleMeasurement"/> object.
    /// </summary>
    /// <returns>
    /// A <see cref="SingleMeasurement"/> object containing the baseline data for the four channels (230nm, 260nm, 280nm, and 340nm).
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if any of the response values from the device cannot be parsed as integers.
    /// </exception>
    /// <exception cref="Exception">
    /// Thrown if the device response is invalid or does not contain the expected number of values.
    /// </exception>
    /// <remarks>
    /// This command expects an empty cuvette guide. If required, the module executes a <see cref="Levelling"/> command internally and then starts a measurement.
    /// if needed and starts after wards a measurement.
    /// </remarks>
    public SingleMeasurement Baseline()
    {
        string[] response = Command("G");

        var channel230 = new Channel(int.Parse(response[1]), int.Parse(response[2]));
        var channel260 = new Channel(int.Parse(response[3]), int.Parse(response[4]));
        var channel280 = new Channel(int.Parse(response[5]), int.Parse(response[6]));
        var channel340 = new Channel(int.Parse(response[7]), int.Parse(response[8]));

        return new SingleMeasurement(channel230, channel260, channel280, channel340);
    }

    /// <summary>
    /// Performs a self-test on the device and retrieves the result.
    /// </summary>
    /// <returns>
    /// A <see cref="SelfTestResult"/> object containing the parsed self-test result.
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if the response value from the device cannot be parsed as an integer.
    /// </exception>
    /// <exception cref="IndexOutOfRangeException">
    /// Thrown if the device response does not contain the expected number of values.
    /// </exception>
    public SelfTestResult SelfTest()
    {
        string[] response = Command("Y");
        return new SelfTestResult(int.Parse(response[1]));
    }

    /// <summary>
    /// Checks whether the cuvette holder is empty.
    /// </summary>
    /// <returns>
    /// <c>true</c> if the cuvette holder is empty; otherwise, <c>false</c>.
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if the response value from the device cannot be parsed as an integer.
    /// </exception>
    /// <exception cref="IndexOutOfRangeException">
    /// Thrown if the device response does not contain the expected number of values.
    /// </exception>
    public bool IsCuvetteHolderEmpty()
    {
        string[] response = Command("X");
        return int.Parse(response[1]) == 1;
    }

    /// <summary>
    /// Performs a measurement and retrieves data from the device.
    /// </summary>
    /// <returns>
    /// A <see cref="SingleMeasurement"/> object containing the measured data for four channels.
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if any of the response values from the device cannot be parsed as integers.
    /// </exception>
    /// <exception cref="IndexOutOfRangeException">
    /// Thrown if the device response does not contain the expected number of values.
    /// </exception>
    public SingleMeasurement Measure()
    {
        string command = "M";

        string[] response = Command(command);
        return new SingleMeasurement(
            new Channel(int.Parse(response[1]), int.Parse(response[2])),
            new Channel(int.Parse(response[3]), int.Parse(response[4])),
            new Channel(int.Parse(response[5]), int.Parse(response[6])),
            new Channel(int.Parse(response[7]), int.Parse(response[8]))
        );
    }

    /// <summary>
    /// Verifies if the 2nd firmware image is valid.
    /// </summary>
    /// <returns>
    /// <c>true</c> if the verification was successful; otherwise, <c>false</c>.
    /// </returns>
    /// <exception cref="FormatException">
    /// Thrown if the response value from the device cannot be parsed as an integer.
    /// </exception>
    /// <exception cref="IndexOutOfRangeException">
    /// Thrown if the device response does not contain the expected number of values.
    /// </exception>
    public bool Verify()
    {
        string[] response = Command("T");
        return int.Parse(response[1]) == 1;
    }

    /// <summary>
    /// Callback signature to report rebooting.
    /// </summary>
    /// <param name="ongoing">
    /// True if a rebooting process is active.
    /// This can be usefull because during rebooting the USB device disapears for a few seconds.
    /// </param>
    public delegate void Rebooting(bool ongoing);

    /// <summary>
    /// Reboots the device.
    /// </summary>
    /// <remarks>
    /// After a restart, the unit may not respond for about 20 seconds.
    /// </remarks>
    private void reboot()
    {
        Command("R");
    }

    /// <summary>
    /// Erases the 2nd firmware image.
    /// The method sends the "F" command to the device, instructing it to erase stored data.
    /// </summary>
    private void erase()
    {
        Command("F");
    }

    /// <summary>
    /// Callback signature to report firmware update progress.
    /// </summary>
    /// <param name="percent">
    /// The overall progress percentage of the update operation, as an integer in the range 0–100.
    /// Values are best-effort estimates and may advance non-linearly.
    /// </param>
    public delegate void UpdateProgress(int percent);

    /// <summary>
    /// Performs a firmware update on the device using a firmware image file.
    /// </summary>
    /// <param name="filename">
    /// Path to the firmware file that contains the new firmware image. The file is read entirely
    /// and each line is transmitted to the device.
    /// </param>
    /// <param name="progress">
    /// Optional callback that receives periodic progress updates (0–100). Progress is reported
    /// after an initial calibration period and during the post-reboot wait.
    /// </param>
    /// <param name="rebooting">
    /// Optional callback that singals when the device reboots.
    /// </param>
    /// <exception cref="System.IO.IOException">
    /// Thrown if the firmware file cannot be read.
    /// </exception>
    /// <exception cref="System.Exception">
    /// Thrown if the firmware update fails at any stage (e.g., verification fails before or after reboot).
    /// </exception>
    /// <remarks>
    /// This overload loads all lines from <paramref name="filename"/> and delegates to
    /// <see cref="FwUpdate(string[], UpdateProgress?, Rebooting ?)"/> for the actual update logic.
    /// </remarks>
    public void FwUpdate(string filename, UpdateProgress? progress = null, Rebooting? rebooting = null)
    {
        string[] lines = System.IO.File.ReadAllLines(filename);
        FwUpdate(lines, progress, rebooting);
    }

    /// <summary>
    /// Performs a firmware update on the device using a string array.
    /// </summary>
    /// <param name="lines">
    /// The firmware image split into lines. Each line is sent to the device using the
    /// <c>"S {line}"</c> command.
    /// </param>
    /// <param name="progress">
    /// Optional callback that receives periodic progress updates (0–100). Progress is reported
    /// after an initial calibration period and during the post-reboot wait.
    /// </param>
    /// <param name="rebooting">
    /// Optional callback that singals when the device reboots.
    /// </param>
    /// <exception cref="System.IO.IOException">
    /// Thrown if the firmware file cannot be read.
    /// </exception>
    /// <exception cref="System.Exception">
    /// Thrown if the firmware update fails at any stage (e.g., verification fails before or after reboot).
    /// </exception>
    public void FwUpdate(string[] lines, UpdateProgress? progress = null, Rebooting? rebooting = null)
    {
        var serialNumber = SerialNumber();
        int delayTime = 30000;
        long duration = 0;
        bool first = true;
        bool valid = false;
        Stopwatch stopWatch = new Stopwatch();

        void UpdateProgress()
        {
            if (progress != null && valid)
            {
                double percent = (double)stopWatch.ElapsedMilliseconds / (double)duration * 100.0;
                if (percent > 100.0)
                {
                    percent = 100.0;
                }
                progress((int)percent);
            }
        }

        erase();

        stopWatch.Start();
        for (int i = 0; i < lines.Length; i++)
        {
            Command($"S {lines[i]}");
            if (first && stopWatch.ElapsedMilliseconds >= 1000)
            {
                first = false;
                if (i > 0)
                {
                    duration = (lines.Length / i) * 1000 + delayTime;
                    valid = true;
                }
            }
            if (valid && (i % 50 == 0))
            {
                UpdateProgress();
            }
        }

        if (!Verify())
        {
            throw new Exception($"Firmware update failed. Image not valid!");
        }

        if (rebooting != null)
        {
            rebooting(true);
        }
        reboot();
        if (serialPort_ != null)
        {
            serialPort_.Close();
        }

        Stopwatch stopWatchDelay = new Stopwatch();
        stopWatchDelay.Start();

        while (true)
        {
            UpdateProgress();

            if (stopWatchDelay.ElapsedMilliseconds >= delayTime)
            {
                break;
            }
            System.Threading.Thread.Sleep(500);
        }

        //In some cases the device has after the reboot an other COM port...
        if (serialPort_ != null)
        {
            var newPortName = DeviceFinder.FindDevice(serialNumber);
            if (newPortName != null)
            {
                serialPort_.PortName = newPortName;
            }
            serialPort_.Open();
            serialPort_.DiscardInBuffer();
            serialPort_.DiscardOutBuffer();
        }

        if (rebooting != null)
        {
            rebooting(false);
        }

        if (Verify())
        {
            throw new Exception($"Firmware update failed. Image still valid!");
        }
    }

    /// <summary>
    /// Adds detailed self-test measurement data to a given JSON node.
    /// The method retrieves various self-test parameters from the device and stores them in the provided
    /// JSON structure, including amplifier values and LED measurement data for different wavelengths.
    /// </summary>
    /// <param name="node">
    /// A <see cref="JsonNode"/> object where the self-test details will be stored.
    /// </param>
    /// <exception cref="ArgumentNullException">
    /// Thrown if the provided <paramref name="node"/> is <c>null</c>.
    /// </exception>
    /// <remarks>
    /// The method follows these steps:
    /// 1. Retrieves amplifier-related self-test values and adds them to the JSON node.
    /// 2. Uses the helper method <c>LedAsJson</c> to fetch and structure LED-specific measurement data for
    ///    different wavelengths (230nm, 260nm, 280nm, and 340nm).
    /// 3. Each LED entry contains:
    ///    - ILED (LED intensity)
    ///    - Dark Sample
    ///    - Dark Reference
    ///    - Sample
    ///    - Reference
    /// </remarks>
    public void AddSelfTestDetails(JsonNode? node)
    {
        JsonNode LedAsJson(Index ILed, Index DarkSample, Index DarkReference, Index Sample, Index Reference)
        {
            JsonObject obj = new JsonObject();

            obj[Dict.SELFTEST_ILED] = Get<int>(ILed);
            obj[Dict.SELFTEST_DARKSAMPLE] = Get<int>(DarkSample);
            obj[Dict.SELFTEST_DARKREFERENCE] = Get<int>(DarkReference);
            obj[Dict.SELFTEST_SAMPLE] = Get<int>(Sample);
            obj[Dict.SELFTEST_REFERENCE] = Get<int>(Reference);

            return obj;
        }

        if (node == null) throw new ArgumentNullException(nameof(node));

        node[Dict.SELFTEST_SPLITRATION230NM] = Get<int>(Index.SELFTEST_AMPLIFIER_SPLITRATIO230NM);
        node[Dict.SELFTEST_AMPLIFIER_CURRENT] = Get<int>(Index.SELFTEST_AMPLIFIER_CURRENT);
        node[Dict.SELFTEST_AMPLIFIER_SAMPLE1] = Get<int>(Index.SELFTEST_AMPLIFIER_SAMPLE1);
        node[Dict.SELFTEST_AMPLIFIER_SAMPLE11] = Get<int>(Index.SELFTEST_AMPLIFIER_SAMPLE11);
        node[Dict.SELFTEST_AMPLIFIER_SAMPLE111] = Get<int>(Index.SELFTEST_AMPLIFIER_SAMPLE111);
        node[Dict.SELFTEST_AMPLIFIER_REFERENCE1] = Get<int>(Index.SELFTEST_AMPLIFIER_REFERENCE1);
        node[Dict.SELFTEST_AMPLIFIER_REFERENCE11] = Get<int>(Index.SELFTEST_AMPLIFIER_REFERENCE11);
        node[Dict.SELFTEST_AMPLIFIER_REFERENCE111] = Get<int>(Index.SELFTEST_AMPLIFIER_REFERENCE111);

        node[Dict.CH_230] = LedAsJson(Index.SELFTEST_LED230_ILED, Index.SELFTEST_LED230_DARKSAMPLE, Index.SELFTEST_LED230_DARKREFERENCE, Index.SELFTEST_LED230_SAMPLE, Index.SELFTEST_LED230_REFERENCE);
        node[Dict.CH_260] = LedAsJson(Index.SELFTEST_LED260_ILED, Index.SELFTEST_LED260_DARKSAMPLE, Index.SELFTEST_LED260_DARKREFERENCE, Index.SELFTEST_LED260_SAMPLE, Index.SELFTEST_LED260_REFERENCE);
        node[Dict.CH_280] = LedAsJson(Index.SELFTEST_LED280_ILED, Index.SELFTEST_LED280_DARKSAMPLE, Index.SELFTEST_LED280_DARKREFERENCE, Index.SELFTEST_LED280_SAMPLE, Index.SELFTEST_LED280_REFERENCE);
        node[Dict.CH_340] = LedAsJson(Index.SELFTEST_LED340_ILED, Index.SELFTEST_LED340_DARKSAMPLE, Index.SELFTEST_LED340_DARKREFERENCE, Index.SELFTEST_LED340_SAMPLE, Index.SELFTEST_LED340_REFERENCE);
    }

    /// <summary>
    /// Generates a technical report containing various diagnostic data from the device.
    /// The report includes levelling results, self-test data, serial number, and firmware version.
    /// </summary>
    /// <returns>
    /// A <see cref="JsonNode"/> object containing structured diagnostic data.
    /// </returns>
    /// <remarks>
    /// The generated report consists of the following fields:
    /// - <see cref="Dict.LEVELLING"/>: The levelling results in JSON format.
    /// - <see cref="Dict.SELFTEST"/>: The base self-test results in JSON format, with additional details added via <see cref="AddSelfTestDetails"/>.
    /// - <see cref="Dict.SERIALNUMBER"/>: The device's serial number.
    /// - <see cref="Dict.FIRMWAREVERSION"/>: The firmware version currently installed on the device.
    /// - <see cref="Dict.PRODUCTIONNUMBER"/>: The device's production number.
    /// </remarks>
    public JsonNode TechnicalReport()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.LEVELLING] = Levelling().ToJson();
        obj[Dict.SELFTEST] = SelfTest().ToJson();
        AddSelfTestDetails(obj[Dict.SELFTEST]);
        obj[Dict.SERIALNUMBER] = SerialNumber();
        obj[Dict.FIRMWAREVERSION] = FirmwareVersion();
        obj[Dict.PRODUCTIONNUMBER] = ProductionNumber();

        return obj;
    }

    /// <summary>
    /// Returns a string representation of the device, including its serial port, serial number, and firmware version.
    /// </summary>
    /// <returns>
    /// A formatted string containing the device's port name, serial number, and firmware version.
    /// </returns>
    /// <remarks>
    /// The returned string follows the format:
    /// "eviDense Module@{PortName} SN:{SerialNumber} Version:{FirmwareVersion}"
    /// Example output:
    /// "eviDense Module@COM3 SN:12345678 Version:1.0.5"
    /// </remarks>
    public override string ToString()
    {
        var portName = "?";
        if (serialPort_ != null)
            portName = this.serialPort_.PortName;

        return $"eviFluor Module@{portName} SN:{serialNumber_} Version:{firmwareVersion_}";
    }

    /// <summary>
    /// Sets the status led color.
    /// </summary>
    public void SetStatusLed(StatusLedColor color)
    {
        Command($"Z {(int)color}");
    }

    /// <summary>
    /// Sets the current of the given LED. Current is in uA.
    /// </summary>
    public void SetLedAndCurrent(Led led, int current)
    {
        Command($"L {(int)led} {current}");
    }

    /// <summary>
    /// Set the amplification factor of the given amplifier.
    /// </summary>
    public void SetAmplifiyingFactor(Amplifier amplifier, AmplifierFactors factor)
    {
        Command($"D {(int)amplifier} {(int)factor}");
    }

    /// <summary>
    /// Get ADC values.
    /// </summary>
    public AdcResult GetAdcValues()
    {
        string[] response = Command("A");
        return new AdcResult(int.Parse(response[1]), int.Parse(response[2]), int.Parse(response[3]), int.Parse(response[4]));
    }

    /// <summary>
    /// Sets the status led color.
    /// </summary>
    /// <exception cref="IndexOutOfRangeException">
    /// Thrown if the device response does not contain the expected number of values.
    /// </exception>
    public List<string> Logging()
    {
        List<string> messages = new List<string> { };
        while (true)
        {
            try
            {
                string[] response = Command("Q");
                messages.Add(response[1]);
            }
            catch (Exception)
            {
                break;
            }
        }

        return messages;
    }

}