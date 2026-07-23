# Python High-Level API

## 1. Scope

This chapter describes the high-level Python API based on the [`Run`][run-api] class.
It is intended for applications that want a guided measurement workflow instead of manually controlling each low-level step.

## 2. Overview

[`Run`][run-api] wraps the repeated measurement sequence and manages:

- device access
- measurement state
- blank handling
- result recalculation
- persistence of measurement data
- persistence of run state
- kit import and export

Import path:

```python
from hse.evidense.run import Run
```

[`Run`][run-api] is the recommended API when the application wants to execute a standard workflow with minimal boilerplate.

## 3. Typical High-Level Example

The following example demonstrates a complete high-level workflow:

```python
from hse.evidense.run import Run


def main():
    run = Run(nr_of_blanks=2)

    samples = ["blank 1", "blank 2", "sample 1", "sample 2"]

    for sample in samples:
        # The liquid handler aspirates at least 10 uL from the current blank or sample well.
        # The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
        if not run.check_empty():
            raise RuntimeError("Cuvette holder must be empty before the air measurement")
        # Start the baseline measurement.
        run.measure()
        # Move the cuvette into the cuvette guide and start the air measurement.
        run.measure()
        # Dispense the liquid into the cuvette and start the sample measurement.
        run.measure(comment = sample)
        # Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.

    run.export_as_csv()


if __name__ == "__main__":
    main()
```

## 4. When to Use `Run`

Use [`Run`][run-api] when:

- you want a guided measurement workflow
- you want automatic blank handling and recalculation
- you want automatic persistence after each step
- you want to resume a run later from saved state

Use the low-level API instead when:

- you need full control over each device command
- you want to manage baseline, air, and sample steps explicitly
- you need custom sequencing beyond the built-in state machine

See also:

- [Python Low-Level API](./python-low-level.md)
- [Python Command Line Interface](./python-cli.md)

## 5. Constructor

Create a new run with:

```python
run = Run(
    nr_of_blanks=2,
    path="data",
    filename="run_data.json",
)
```

Important constructor arguments:

- `nr_of_blanks`: number of blank measurements used to derive factors
- `path`: optional directory for the measurement file
- `filename`: optional measurement JSON file name
- `device`: optional serial number or an already created device object
- `no_purity_ratio_260_280_correction`: disables wavelength-based correction

Behavior:

- if no filename is given, a timestamped JSON filename is generated
- the [`Run`][run-api] instance creates a [`StorageMeasurement`][storage-api] internally
- device metadata and run parameters are added automatically by default
- when purity-ratio correction is enabled, device settings are read automatically
- if `nr_of_blanks` is `0`, factors are not derived from blank measurements and a kit can be loaded later with [`run.import_kit(...)`][run-importkit-api]

## 6. Run State Model

[`Run`][run-api] keeps an internal state machine:

1. [`BASELINE`][run-state-api]
2. [`AIR`][run-state-api]
3. [`SAMPLE`][run-state-api]

Each call to [`run.measure(...)`][run-measure-api] advances the workflow by one step.

Practical effect:

- first call performs the baseline acquisition
- second call performs the air acquisition
- third call performs the sample acquisition and appends a completed [`Measurement`][measurement-api] to storage

After the sample step, the state returns to `BASELINE`.

## 7. Blank Handling and Recalculation

[`Run`][run-api] derives correction factors automatically once enough blank measurements are available.

Behavior:

- until `nr_of_blanks` completed measurements exist, stored measurements may not yet contain calculated results
- once the configured number of blanks is available, factors are calculated
- all stored measurements are recalculated automatically

This makes `Run` suitable for workflows where the blank measurements are collected first and the sample results become available afterwards.

## 8. Persisted Files

[`Run`][run-api] manages two kinds of files:

- the measurement JSON file
- the run state JSON file

The measurement JSON file contains:

- device information
- run parameters
- optional device adjustments
- completed measurements
- calculated results

The run state file contains:

- current run state
- current count
- selected device
- partially collected baseline, air, or sample data
- factors, if available
- the measurement filename

## 9. Saving and Loading Run State

Use [`save_state()`][run-savestate-api] to persist the current run state:

```python
run.save_state()
```

Or save to an explicit file:

```python
run.save_state("my-state.json")
```

Resume later with [`Run.load_state(...)`][run-loadstate-api]:

```python
run = Run.load_state("my-state.json")
```

If no explicit state filename is provided, [`Run`][run-api] resolves a default state filename based on the selected device.

This allows workflows such as:

- initialize a run in one process
- continue the run in later CLI invocations or scripts
- recover after application restart

## 10. Checking the Cuvette Holder

Use [`run.check_empty()`][run-checkempty-api]:

```python
empty = run.check_empty()
```

This forwards to the underlying device and returns `True` when the cuvette holder is empty.

## 11. Exporting Data

Export the active measurement file as CSV with [`run.export_as_csv()`][run-exportascsv-api]:

```python
run.export_as_csv()
```

This uses the internally managed measurement filename.

## 12. Kit Import and Export

Export a kit from the run with [`run.export_as_kit(...)`][run-exportaskit-api]:

```python
run.export_as_kit("kit.json", comment="Batch A")
```

Import a kit into the run with [`run.import_kit(...)`][run-importkit-api]:

```python
run.import_kit("kit.json")
```

Typical use cases:

- reuse factors from a previous blank run
- separate blank preparation from sample processing
- prepare a kit file for later production workflows

[run-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run
[run-state-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.State
[storage-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.storage.html#hse.evidense.storage.StorageMeasurement
[measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.measurement.html#hse.evidense.measurement.Measurement
[run-measure-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.measure
[run-savestate-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.save_state
[run-loadstate-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.load_state
[run-checkempty-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.check_empty
[run-exportascsv-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.export_as_csv
[run-exportaskit-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.export_as_kit
[run-importkit-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.run.html#hse.evidense.run.Run.import_kit
