# C# Low-Level API

## 1. Scope

This chapter describes the low-level C# API.
It intentionally does not use the [`Run`][run-api] class, because the goal is to show the explicit device workflow.

## 2. API Overview

The low-level API is centered around explicit interaction with a [`Device`][device-api] object and related data classes.

Main objects to document:

- [`Device`][device-api]
- [`SingleMeasurement`][single-measurement-api]
- [`Measurement`][measurement-api]
- [`DeviceSettings`][device-settings-api]
- storage-related classes such as [`StorageMeasurement`][storage-measurement-api]

Import path:

```csharp
using Hse.EviDense;
```

Important low-level [`Device`][device-api] members:

- [`new Device(string? serialNumber = null)`][device-api]
- [`Device.GetAvailableDevices()`][device-getavailabledevices-api]
- [`device.SerialNumber()`][device-serialnumber-api]
- [`device.FirmwareVersion()`][device-firmwareversion-api]
- [`device.ProductionNumber()`][device-productionnumber-api]
- [`device.SelfTest()`][device-selftest-api]
- [`device.IsCuvetteHolderEmpty()`][device-iscuvetteholderempty-api]
- [`device.Baseline()`][device-baseline-api]
- [`device.Measure()`][device-measure-api]
- [`device.Logging()`][device-logging-api]
- `device.Dispose()`

## 3. Complete Low-Level Example

The following example demonstrates the full low-level workflow without [`Run`][run-api]:

```csharp
using System;
using System.Collections.Generic;
using Hse.EviDense;

internal class Program
{
    private static void Main()
    {
        using var device = new Device();
        var settings = DeviceSettings.FromDevice(device);
        var storage = new StorageMeasurement();
        storage.AddDeviceInfo(device, "low-level example");

        Console.WriteLine($"serial: {device.SerialNumber()}");
        Console.WriteLine($"firmware: {device.FirmwareVersion()}");

        SelfTestResult selftest = device.SelfTest();
        if (selftest.HasProblems())
        {
            throw new InvalidOperationException($"Selftest failed with code {selftest.Result}");
        }

        Measurement AcquireMeasurement(string comment)
        {
            // The liquid handler aspirates at least 10 uL from the current blank or sample well.
            // The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
            if (!device.IsCuvetteHolderEmpty())
            {
                throw new InvalidOperationException("Cuvette holder is not empty");
            }

            // Start the baseline measurement.
            SingleMeasurement baseline = device.Baseline();
            // Move the cuvette into the cuvette guide and start the air measurement.
            SingleMeasurement air = device.Measure();
            // Dispense the liquid into the cuvette and start the sample measurement.
            SingleMeasurement sample = device.Measure();
            // Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.
            return new Measurement(baseline, air, sample, comment);
        }

        var blankMeasurements = new List<Measurement>();
        foreach (string blankName in new[] { "blank 1", "blank 2" })
        {
            blankMeasurements.Add(AcquireMeasurement(blankName));
        }

        var factors = new Factors();
        foreach (Measurement measurement in blankMeasurements)
        {
            factors += measurement.Factors();
        }
        factors /= blankMeasurements.Count;

        foreach (Measurement measurement in blankMeasurements)
        {
            var results = measurement.Results(factors, deviceSettings: settings);
            storage.AppendWithResults(measurement, results, measurement.Comment(), device.Logging());
        }

        var sampleMeasurements = new List<Measurement>();
        foreach (string sampleName in new[] { "sample 1", "sample 2" })
        {
            sampleMeasurements.Add(AcquireMeasurement(sampleName));
        }

        foreach (Measurement measurement in sampleMeasurements)
        {
            var results = measurement.Results(factors, deviceSettings: settings);
            storage.AppendWithResults(measurement, results, measurement.Comment(), device.Logging());
        }

        storage.Save("run_data.json");
    }
}
```

## 4. Opening a Device

Available devices can be enumerated with [`Device.GetAvailableDevices()`][device-getavailabledevices-api]:

```csharp
var devices = Device.GetAvailableDevices();
```

Then create a device either by serial number or by auto-detection:

```csharp
using var device = new Device();
using var deviceBySerial = new Device("SN0010");
```

Recommendations:

- use `using` so the serial connection is disposed reliably
- surface constructor failures clearly, because they usually indicate connection or discovery problems

## 5. Querying Device Information

Basic device metadata can be queried directly:

```csharp
using var device = new Device();

string serialNumber = device.SerialNumber();
string firmwareVersion = device.FirmwareVersion();
string productionNumber = device.ProductionNumber();
```

The library version can be queried separately with [`Device.LibraryVersion`][device-libraryversion-api]:

```csharp
string libraryVersion = Device.LibraryVersion;
```

## 6. Running a Self-Test

Run the self-test with [`device.SelfTest()`][device-selftest-api]:

```csharp
SelfTestResult result = device.SelfTest();
```

The returned [`SelfTestResult`][selftestresult-api] object provides:

- [`result.Result`][selftestresult-result-api]
- [`result.HasProblems()`][selftestresult-hasproblems-api]
- per-condition helpers such as [`HasProblemWithILed230()`][selftestresult-hasproblemwithiled230-api] or [`HasProblemWithSample260()`][selftestresult-hasproblemwithsample260-api]

Example:

```csharp
SelfTestResult result = device.SelfTest();
if (result.HasProblems())
{
    throw new InvalidOperationException($"Selftest failed with code {result.Result}");
}
```

## 7. Acquiring Raw Measurements

The explicit low-level measurement workflow is:

1. [`Baseline()`][device-baseline-api] with an empty cuvette guide
2. [`Measure()`][device-measure-api] for the air measurement
3. [`Measure()`][device-measure-api] for the sample measurement

Example:

```csharp
SingleMeasurement baseline = device.Baseline();
SingleMeasurement air = device.Measure();
SingleMeasurement sample = device.Measure();
```

You can check the cuvette holder state before starting with [`device.IsCuvetteHolderEmpty()`][device-iscuvetteholderempty-api]:

```csharp
bool empty = device.IsCuvetteHolderEmpty();
```

Each [`SingleMeasurement`][single-measurement-api] contains four wavelength channels:

- 230 nm
- 260 nm
- 280 nm
- 340 nm

## 8. Building a `Measurement`

Create a higher-level [`Measurement`][measurement-api] object from the three acquisitions:

```csharp
var measurement = new Measurement(baseline, air, sample, "sample A");
```

This separates raw acquisition from result calculation and storage.

## 9. Calculating Results

Absorbance values can be calculated with [`measurement.Absorbance()`][measurement-absorbance-api]:

```csharp
var absorbance = measurement.Absorbance();
```

Calculated assay results can be calculated with [`measurement.Results(...)`][measurement-results-api]:

```csharp
var settings = DeviceSettings.FromDevice(device);
var factors = new Factors();
var results = measurement.Results(factors, deviceSettings: settings);
```

Notes:

- wavelength correction may depend on [`DeviceSettings`][device-settings-api]
- the low-level API keeps factor handling explicit
- in a production workflow, correction factors typically come from blank measurements, not from the current sample
- this is the main difference from [`Run`][run-api], which hides these steps

## 10. Reading Device Settings

Read settings from the connected device with [`DeviceSettings.FromDevice(device)`][device-settings-fromdevice-api]:

```csharp
var settings = DeviceSettings.FromDevice(device);
```

The most relevant values for the user manual are:

- center wavelength for 260 nm
- center wavelength for 280 nm

These settings can influence corrected result calculation.

## 11. Persisting Data

Use [`StorageMeasurement`][storage-measurement-api] for persistence:

```csharp
var storage = new StorageMeasurement();
storage.AddDeviceInfo(device, "manual low-level workflow");
storage.AppendWithResults(measurement, results, "sample A", device.Logging());
storage.Save("run_data.json");
```

Supported operations include:

- create a new storage container
- load a storage file from JSON
- add device metadata
- append measurements with or without results
- save as JSON
- export JSON data as CSV

CSV export example with [`StorageMeasurement.ExportAsCsv(...)`][storage-measurement-exportascsv-api]:

```csharp
StorageMeasurement.ExportAsCsv("run_data.json");
```

## 12. Error Handling and Cleanup

The C# API uses exceptions for communication and workflow failures.

Handle at least these categories:

- device discovery failures
- serial communication failures
- self-test failures according to [`SelfTestResult`][selftestresult-api]
- invalid file paths during persistence

Always dispose the [`Device`][device-api] instance:

```csharp
using var device = new Device();
```

This is especially important for real serial devices.

## 13. Notes About `Run`

The [`Run`][run-api] class is intentionally excluded from this chapter because it abstracts away the individual device operations.

[run-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html
[device-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html
[single-measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SingleMeasurement.html
[measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Measurement.html
[device-settings-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.DeviceSettings.html
[storage-measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.StorageMeasurement.html
[selftestresult-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SelfTestResult.html
[device-getavailabledevices-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_GetAvailableDevices
[device-serialnumber-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_SerialNumber
[device-firmwareversion-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_FirmwareVersion
[device-productionnumber-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_ProductionNumber
[device-selftest-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_SelfTest
[device-iscuvetteholderempty-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_IsCuvetteHolderEmpty
[device-baseline-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_Baseline
[device-measure-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_Measure
[device-logging-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_Logging
[device-libraryversion-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Device.html#Hse_EviDense_Device_LibraryVersion
[device-settings-fromdevice-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.DeviceSettings.html#Hse_EviDense_DeviceSettings_FromDevice_Hse_EviDense_Device_
[selftestresult-result-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SelfTestResult.html#Hse_EviDense_SelfTestResult_Result
[selftestresult-hasproblems-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SelfTestResult.html#Hse_EviDense_SelfTestResult_HasProblems
[selftestresult-hasproblemwithiled230-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SelfTestResult.html#Hse_EviDense_SelfTestResult_HasProblemWithILed230
[selftestresult-hasproblemwithsample260-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.SelfTestResult.html#Hse_EviDense_SelfTestResult_HasProblemWithSample260
[measurement-absorbance-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Measurement.html#Hse_EviDense_Measurement_Absorbance_Hse_EviDense_Quadruple_Hse_EviDense_DeviceSettings_
[measurement-results-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Measurement.html#Hse_EviDense_Measurement_Results_Hse_EviDense_Factors_System_Double_Hse_EviDense_DeviceSettings_
[storage-measurement-exportascsv-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.StorageMeasurement.html#Hse_EviDense_StorageMeasurement_ExportAsCsv_System_String_
