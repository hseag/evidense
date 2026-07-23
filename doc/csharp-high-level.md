# C# High-Level API

## 1. Scope

This chapter describes the high-level C# API based on the [`Run`][run-api] class.
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

```csharp
using Hse.EviDense;
```

[`Run`][run-api] is the recommended API when the application wants to execute a standard workflow with minimal boilerplate.

## 3. Typical High-Level Example

The following example demonstrates a complete high-level workflow:

```csharp
using System;
using Hse.EviDense;

internal class Program
{
    private static void Main()
    {
        using var run = new Run(nrOfBlanks: 2);

        string[] samples = [ "blank 1", "blank 2", "sample 1", "sample 2" ];

        foreach (string sample in samples)
        {
            // The liquid handler aspirates at least 10 uL from the current blank or sample well.
            // The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
            if (!run.checkEmpty())
            {
                throw new InvalidOperationException("Cuvette holder must be empty before the air measurement");
            }

            // Start the baseline measurement.
            run.measure();
            // Move the cuvette into the cuvette guide and start the air measurement.
            run.measure();
            // Dispense the liquid into the cuvette and start the sample measurement.
            run.measure(sample);
            // Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.
        }

        run.ExportAsCsv();
    }
}
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

- [C# Low-Level API](./csharp-low-level.md)

## 5. Constructor

Create a new run with:

```csharp
using var run = new Run(
    nrOfBlanks: 2);
```

Important constructor arguments:

- `nrOfBlanks`: number of blank measurements used to derive factors
- `path`: optional directory for the measurement file
- `filename`: optional measurement JSON file name
- `device`: optional serial number or an already created device object
- `noPurityRatio260280Correction`: disables wavelength-based correction

Behavior:

- if no filename is given, a timestamped JSON filename is generated
- the [`Run`][run-api] instance creates a [`StorageMeasurement`][storage-measurement-api] internally
- device metadata and run parameters are added automatically by default
- when purity-ratio correction is enabled, device settings are read automatically
- if `nrOfBlanks` is `0`, factors are not derived from blank measurements and a kit can be loaded later with [`run.ImportKit(...)`][run-importkit-api]

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

- until `nrOfBlanks` completed measurements exist, stored measurements may not yet contain calculated results
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

Use [`SaveState()`][run-savestate-api] to persist the current run state:

```csharp
run.SaveState();
```

Or save to an explicit file:

```csharp
run.SaveState("my-state.json");
```

Resume later with [`Run.LoadState(...)`][run-loadstate-api]:

```csharp
using var run = Run.LoadState("my-state.json");
```

If no explicit state filename is provided, [`Run`][run-api] resolves a default state filename based on the selected device.

## 10. Checking the Cuvette Holder

Use [`run.checkEmpty()`][run-checkempty-api]:

```csharp
bool empty = run.checkEmpty();
```

This forwards to the underlying device and returns `true` when the cuvette holder is empty.

## 11. Exporting Data

Export the active measurement file as CSV with [`run.ExportAsCsv()`][run-exportascsv-api]:

```csharp
run.ExportAsCsv();
```

This uses the internally managed measurement filename.

## 12. Kit Import and Export

Export a kit from the run with [`run.ExportAsKit(...)`][run-exportaskit-api]:

```csharp
run.ExportAsKit("kit.json", "Batch A");
```

Import a kit into the run with [`run.ImportKit(...)`][run-importkit-api]:

```csharp
run.ImportKit("kit.json");
```

Typical use cases:

- reuse factors from a previous blank run
- separate blank preparation from sample processing
- prepare a kit file for later production workflows

[run-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html
[run-state-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.RunState.html
[storage-measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.StorageMeasurement.html
[measurement-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Measurement.html
[run-measure-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_measure_System_String_
[run-savestate-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_SaveState_System_String_
[run-loadstate-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_LoadState_System_String_
[run-checkempty-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_checkEmpty
[run-exportascsv-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_ExportAsCsv
[run-exportaskit-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_ExportAsKit_System_String_System_String_
[run-importkit-api]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.Run.html#Hse_EviDense_Run_ImportKit_System_String_
