# Python Low-Level API

## 1. Scope

This chapter describes the low-level Python API.
It intentionally does not use the [`Run`][run-api] class, because the goal is to show the explicit device workflow.

## 2. API Overview

The low-level Python API is centered around explicit interaction with a [`Device`][device-api] object and related classes.

Main objects to document:

- [`Device`][device-api]
- [`SingleMeasurement`][singlemeasurement-api]
- [`Measurement`][measurement-api]
- [`DeviceSettings`][devicesettings-api]
- storage-related classes such as [`StorageMeasurement`][storage-api]

Import path:

```python
from hse.evidense.device import Device
from hse.evidense.devicesettings import DeviceSettings
from hse.evidense.measurement import Factors, Measurement
from hse.evidense.storage import StorageMeasurement
```

Important low-level [`Device`][device-api] methods:

- [`Device(device=None)`][device-api] opens a physical device
- [`Device.find_device()`][device-finddevice-api] searches for connected devices
- [`Device.serial_number()`][device-serial-api]
- [`Device.firmware_version()`][device-fw-api]
- [`Device.production_number()`][device-prod-api]
- [`Device.selftest()`][device-selftest-api]
- [`Device.is_cuvette_holder_empty()`][device-empty-api]
- [`Device.baseline()`][device-baseline-api]
- [`Device.measure()`][device-measure-api]
- [`Device.logging()`][device-logging-api]

## 3. Complete Low-Level Example

The following example demonstrates the full low-level workflow without [`Run`][run-api]:

```python
from hse.evidense.device import Device
from hse.evidense.devicesettings import DeviceSettings
from hse.evidense.measurement import Factors, Measurement
from hse.evidense.storage import StorageMeasurement


def main():
    device = Device()
    settings = DeviceSettings.from_device(device)
    storage = StorageMeasurement()
    storage.add_device_info(device, comment="low-level example")

    print("serial:", device.serial_number())
    print("firmware:", device.firmware_version())

    selftest = device.selftest()
    if selftest.has_problems():
        raise RuntimeError(f"Selftest failed with code {selftest.result}")

    def acquire_measurement(comment):
        # The liquid handler aspirates at least 10 uL from the current blank or sample well.
        # The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
        if not device.is_cuvette_holder_empty():
            raise RuntimeError("Cuvette holder is not empty")

        # Start the baseline measurement.
        baseline = device.baseline()
        # Move the cuvette into the cuvette guide and start the air measurement.
        air = device.measure()
        # Dispense the liquid into the cuvette and start the sample measurement.
        sample = device.measure()
        # Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.
        return Measurement(baseline, air, sample, comment=comment)

    blank_measurements = []
    for blank_name in ["blank 1", "blank 2"]:
        blank_measurements.append(acquire_measurement(blank_name))

    factors = Factors()
    for measurement in blank_measurements:
        factors = factors + measurement.factors()
    factors = factors / len(blank_measurements)

    for measurement in blank_measurements:
        results = measurement.results(factors=factors, device_settings=settings)
        storage.append_with_results(
            measurement,
            results,
            comment=measurement.comment(),
            logging=device.logging(),
        )

    sample_measurements = []
    for sample_name in ["sample 1", "sample 2"]:
        sample_measurements.append(acquire_measurement(sample_name))

    for measurement in sample_measurements:
        results = measurement.results(factors=factors, device_settings=settings)
        storage.append_with_results(
            measurement,
            results,
            comment=measurement.comment(),
            logging=device.logging(),
        )

    storage.save("run_data.json")


if __name__ == "__main__":
    main()
```

## 4. Opening a Device

You can either let the library auto-detect a connected device or pass a specific serial number.

Examples:

```python
device = Device()
device = Device("SN0010")
```

Notes:

- [`Device()`][device-api] searches for a connected device with the expected VID and PID
- when a device cannot be found, the constructor raises an exception

The Python `Device` class does not expose a dedicated context manager.
For a hardware connection, the serial port is opened in the constructor and kept for the lifetime of the object.

## 5. Querying Device Information

Use these methods for basic metadata:

```python
device = Device()

serial_number = device.serial_number()
firmware_version = device.firmware_version()
production_number = device.production_number()
```

Typical uses:

- identify the device in logs and stored files
- display firmware information in a UI or CLI
- add device metadata to persisted measurement data

## 6. Running a Self-Test

Run the self-test with [`device.selftest()`][device-selftest-api]:

```python
result = device.selftest()
```

The returned [`SelfttestResult`][selftest-api] object provides:

- [`result.result`][selftest-result-api]
- [`result.has_problems()`][selftest-hasproblems-api]
- per-channel helpers such as [`has_problems_with_iled_230()`][selftest-iled230-api] or [`has_problems_with_sample_260()`][selftest-sample260-api]

Example:

```python
result = device.selftest()
if result.has_problems():
    raise RuntimeError(f"Selftest failed with code {result.result}")
```

If you need a more detailed technical payload, use:

```python
details = device.selftest_details_as_json()
```

## 7. Acquiring Raw Measurements

The low-level measurement workflow is explicit:

1. call [`baseline()`][device-baseline-api] with an empty cuvette guide
2. call [`measure()`][device-measure-api] to acquire the air measurement
3. call [`measure()`][device-measure-api] again to acquire the sample measurement

Example:

```python
baseline = device.baseline()
air = device.measure()
sample = device.measure()
```

Both [`baseline()`][device-baseline-api] and [`measure()`][device-measure-api] return [`SingleMeasurement`][singlemeasurement-api] objects containing four wavelength channels:

- 230 nm
- 260 nm
- 280 nm
- 340 nm

Each channel contains:

- sample value
- reference value

You can also check the cuvette guide state before starting:

```python
empty = device.is_cuvette_holder_empty()
```

## 8. Building a `Measurement`

Create a [`Measurement`][measurement-api] from the three low-level acquisitions:

```python
measurement = Measurement(baseline, air, sample, comment="sample A")
```

This object groups the raw data and acts as the main input for result calculation and persistence.

Useful accessors:

- `measurement.comment()`
- `measurement.set_comment(...)`
- `measurement.factor_a_buffer_blank()`

## 9. Calculating Results

For absorbance values, use [`measurement.absorbance()`][measurement-absorbance-api]:

```python
absorbance = measurement.absorbance()
```

For calculated assay results, use [`measurement.results(...)`][measurement-results-api]:

```python
settings = DeviceSettings.from_device(device)
factors = Factors()
results = measurement.results(factors=factors, device_settings=settings)
```

Notes:

- [`DeviceSettings.from_device(device)`][devicesettings-fromdevice-api] reads wavelength information from the device
- result calculation can use correction factors and device settings
- in a production workflow, correction factors typically come from blank measurements, not from the current sample
- [`measurement.results(...)`][measurement-results-api] returns a result object suitable for JSON persistence

If your workflow disables the 260/280 correction, describe that decision explicitly in your application logic.

## 10. Reading Device Settings

Read settings directly from the connected device with [`DeviceSettings.from_device(device)`][devicesettings-fromdevice-api]:

```python
settings = DeviceSettings.from_device(device)
```

The most important settings currently documented here are:

- center wavelength at 260 nm
- center wavelength at 280 nm

These settings influence the absorbance-based result calculation, especially when wavelength correction is applied.

Settings can also be reconstructed from stored JSON with [`DeviceSettings.from_json(node)`][devicesettings-fromjson-api]:

```python
settings = DeviceSettings.from_json(node)
```

## 11. Persisting Data

Use [`StorageMeasurement`][storage-api] for JSON persistence:

```python
storage = StorageMeasurement()
storage.add_device_info(device, comment="manual low-level workflow")
storage.append_with_results(measurement, results, comment="sample A", logging=device.logging())
storage.save("run_data.json")
```

Available operations include:

- [`StorageMeasurement()`][storage-api] to create a new container
- [`StorageMeasurement(filename)`][storage-api] to load an existing JSON file
- [`add_device_info(device, comment=None)`][storage-adddeviceinfo-api]
- `append(...)`
- [`append_with_results(...)`][storage-appendwithresults-api]
- [`save(filename)`][storage-save-api]
- [`StorageMeasurement.export_as_csv(filename_json)`][storage-exportcsv-api]

CSV export example:

```python
StorageMeasurement.export_as_csv("run_data.json")
```

## 12. Error Handling and Cleanup

The Python API uses exceptions for device communication and data errors.

Handle at least these cases:

- device not found
- device communication timeout or protocol error
- self-test failure according to the returned self-test result
- invalid file paths during persistence or export

Example:

```python
try:
    device = Device()
    result = device.selftest()
    if result.has_problems():
        raise RuntimeError(f"Selftest failed with code {result.result}")
except Exception as exc:
    print(f"Error: {exc}")
```

Cleanup note:

- for physical devices the serial port is held inside the `Device` instance
- the current API does not document an explicit `close()` method
- keep the `Device` lifetime narrow and release the object when the workflow is complete

## 13. Notes About `Run`

The [`Run`][run-api] class is intentionally excluded from this chapter because it abstracts away the individual device operations.

[run-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run
[device-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device
[singlemeasurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.singlemeasurement.html#hse.evidense.singlemeasurement.SingleMeasurement
[measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.measurement.html#hse.evidense.measurement.Measurement
[devicesettings-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.devicesettings.html#hse.evidense.devicesettings.DeviceSettings
[storage-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement
[selftest-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.SelfttestResult
[device-finddevice-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.find_device
[device-serial-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.serial_number
[device-fw-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.firmware_version
[device-prod-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.production_number
[device-selftest-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.selftest
[device-empty-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.is_cuvette_holder_empty
[device-baseline-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.baseline
[device-measure-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.measure
[device-logging-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.Device.logging
[selftest-result-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.SelfttestResult.result
[selftest-hasproblems-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.SelfttestResult.has_problems
[selftest-iled230-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.SelfttestResult.has_problems_with_iled_230
[selftest-sample260-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.device.html#hse.evidense.device.SelfttestResult.has_problems_with_sample_260
[measurement-absorbance-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.measurement.html#hse.evidense.measurement.Measurement.absorbance
[measurement-results-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.measurement.html#hse.evidense.measurement.Measurement.results
[devicesettings-fromdevice-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.devicesettings.html#hse.evidense.devicesettings.DeviceSettings.from_device
[devicesettings-fromjson-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.devicesettings.html#hse.evidense.devicesettings.DeviceSettings.from_json
[storage-adddeviceinfo-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement.add_device_info
[storage-appendwithresults-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement.append_with_results
[storage-save-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement.save
[storage-exportcsv-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement.export_as_csv
