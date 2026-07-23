# Simulator Guide

## 1. Overview

The repository contains an eviDense UV simulator for development and testing without a physical device.
The simulator exposes the same TCP-based command protocol that the software interfaces use when a device is opened as `"SIMULATION"`.
When started without preloaded data, the simulator behaves like an eviDense UV device with simulated baseline, air, and sample values.

With `LOAD`, an existing measurement JSON file from a real or previously simulated run can be loaded and replayed step by step.
This is useful for repeating known runs during development, regression tests, and interface validation.

The simulator package is located in [`simulator`](../simulator).

## 2. Installation

The simulator is provided as a Python package with the console script `hse-simulator`.

Example installation from the repository root:

```powershell
python -m pip install https://hseag.github.io/evidense/main/simulator/dist/hse_simulator-0.1.0-py3-none-any.whl
```

This installs:

- `hse-simulator`

## 3. Start the eviDense UV Simulator

Start the default eviDense UV simulator with:

```powershell
hse-simulator evidense
```

Behavior:

- the simulator listens on TCP port `5000`
- the matching web UI starts automatically unless `--no-web` is passed
- the default web UI bind host is `127.0.0.1`
- the default web UI port for `evidense` is `8011`

Useful variants:

```powershell
hse-simulator evidense .\test\testdata\evidense-example.json
hse-simulator --no-web evidense
hse-simulator --web-host 127.0.0.1 --web-port 8001 evidense
hse-simulator --verbose evidense
hse-sim-web-evidense --host 127.0.0.1 --port 8011 --sim-host 127.0.0.1 --sim-port 5000
```

Meaning of the most important options:

- `<data file>`: preloads measurement values from a JSON file
- `--no-web`: disables the browser-based control UI
- `--web-host` and `--web-port`: configure where the web UI is exposed
- `--verbose`: prints request and response details

## 4. Interface Examples

After the simulator is running, all supported interfaces can be pointed to the simulated device.
The important selector is the device name `"SIMULATION"`.

### 4.1 C CLI

The C command line tool can run against the simulator by passing `--device SIMULATION`.

Example:

```powershell
evidense-cli --device SIMULATION selftest
evidense-cli --device SIMULATION empty
evidense-cli --device SIMULATION baseline
evidense-cli --device SIMULATION measure
evidense-cli --device SIMULATION run init 2
evidense-cli --device SIMULATION run init 2 --no_purity_ratio_260_280_correction
evidense-cli --device SIMULATION run measure "blank 1"
```

See also [C Command Line Interface](./c-cli.md).

### 4.2 C# Low-Level API

In the low-level C# API, open the device as `"SIMULATION"`:

```csharp
using Hse.EviDense;

using var device = new Device("SIMULATION");

var selftest = device.SelfTest();
bool empty = device.IsCuvetteHolderEmpty();
var baseline = device.Baseline();
var measurement = device.Measure();
```

See also [C# Low-Level API](./csharp-low-level.md).

### 4.3 C# High-Level API

In the high-level C# API, create the run with `device: "SIMULATION"`:

```csharp
using Hse.EviDense;

var run = new Run(
    nrOfBlanks: 2,
    device: "SIMULATION");

if (!run.checkEmpty())
{
    throw new InvalidOperationException("Cuvette guide must be empty before the measurement");
}

run.measure();
run.measure("blank 1");
```

See also [C# High-Level API](./csharp-high-level.md).

### 4.4 Python Low-Level API

In the low-level Python API, open the device as `"SIMULATION"`:

```python
from hse.evidense.device import Device

device = Device("SIMULATION")

selftest = device.selftest()
empty = device.is_cuvette_holder_empty()
baseline = device.baseline()
measurement = device.measure()

device.close()
```

See also [Python Low-Level API](./python-low-level.md).

### 4.5 Python High-Level API

In the high-level Python API, create the run with `device="SIMULATION"`:

```python
from hse.evidense.run import Run

run = Run(
    nr_of_blanks=2,
    device="SIMULATION",
)

if not run.check_empty():
    raise RuntimeError("Cuvette guide must be empty before the measurement")

run.measure()
run.measure("blank 1")
run.close()
```

See also [Python High-Level API](./python-high-level.md).

### 4.6 Python CLI

The Python CLI can run against the simulator with `--device SIMULATION`.

Example:

```powershell
python -m hse.evidense --device SIMULATION info
python -m hse.evidense --device SIMULATION selftest --json
python -m hse.evidense --device SIMULATION checkempty
python -m hse.evidense --device SIMULATION run init 2
python -m hse.evidense --device SIMULATION run measure
python -m hse.evidense --device SIMULATION run measure
python -m hse.evidense --device SIMULATION run measure "sample 1"
```

See also [Python Command Line Interface](./python-cli.md).

### 4.7 Python REST API

First start the REST server:

```powershell
evidense-rest --host 127.0.0.1 --port 8000
```

Then initialize a run against the simulator by using `serial_number: "SIMULATION"`:

```python
from hse.evidense.rest_client import RestClient

client = RestClient(base_url="http://127.0.0.1:8000", serial_number="SIMULATION")

run = client.run_init(
    nr_of_blanks=2,
)
run_id = run["run_id"]

if not client.checkempty()["empty"]:
    raise RuntimeError("Cuvette guide must be empty before the measurement")

client.run_measure(run_id)
client.run_measure(run_id, comment="sample 1")
```

See also [Python REST API](./python-rest.md).

## 5. Web UI

The eviDense UV simulator web UI starts automatically with `hse-simulator evidense` unless `--no-web` is passed.
It exposes a small browser-based control surface for common simulator operations.

The web UI supports:

- reading and changing the `CHECKEMPTY` state
- resetting the simulator
- enabling and disabling `ZERO` mode
- loading JSON measurement files by path
- uploading a JSON file through the browser and loading it immediately

The standalone web UI can also be started separately:

```powershell
hse-sim-web-evidense --host 127.0.0.1 --port 8011 --sim-host 127.0.0.1 --sim-port 5000
```

## 6. Simulator Control Commands

The simulator accepts control commands while it is running.
These commands are useful for test setup and for switching specific states.

Examples:

```powershell
hse-simulator sim RESET
hse-simulator sim CHECKEMPTY 1
hse-simulator sim CHECKEMPTY 0
hse-simulator sim LOAD .\test\testdata\evidense-example.json
hse-simulator sim ZERO 1
hse-simulator sim ZERO 0
hse-simulator sim SKIP 2
```

Typical command usage:

- `RESET`: resets the simulator state
- `CHECKEMPTY 1`: report that the cuvette guide is empty
- `CHECKEMPTY 0`: report that the cuvette guide is not empty
- `LOAD <file>`: load measurement data from a JSON file
- `ZERO 1`: enable zero-value measurement mode
- `ZERO 0`: disable zero-value measurement mode
- `SKIP <count>`: skip the next preloaded measurement entries

## 7. Command Reference

This chapter summarizes the most important simulator control functions.
All commands are sent with `hse-simulator sim ...` while the simulator is running.

### 7.1 `LOAD <file>`

Loads measurement data from a JSON file into the simulator.
The loaded values are then returned step by step during the following baseline and measurement calls.
The file is loaded from the simulator process point of view.
This means the referenced file path must be accessible on the same computer where the simulator is running.

Example:

```powershell
hse-simulator sim LOAD .\test\testdata\evidense-example.json
```

Typical use:

- replay a known measurement run
- reproduce a customer issue with fixed data
- validate a client implementation against stable expected values

### 7.2 `RESET`

Resets the simulator state.
This clears loaded measurement progress, resets the empty state to default, and clears temporary simulator status.

Example:

```powershell
hse-simulator sim RESET
```

Typical use:

- start a test from a clean simulator state
- restart a workflow after a failed test run

### 7.3 `CHECKEMPTY 0|1`

Sets the reported cuvette-guide state.

Examples:

```powershell
hse-simulator sim CHECKEMPTY 1
hse-simulator sim CHECKEMPTY 0
```

Meaning:

- `CHECKEMPTY 1`: the simulator reports that the cuvette guide is empty
- `CHECKEMPTY 0`: the simulator reports that the cuvette guide is not empty

Typical use:

- test empty-check handling in a client
- simulate a blocked or occupied cuvette guide

### 7.4 `ZERO 0|1`

Enables or disables zero-value measurement mode.

Examples:

```powershell
hse-simulator sim ZERO 1
hse-simulator sim ZERO 0
```

Meaning:

- `ZERO 1`: baseline and measurement commands return zero values
- `ZERO 0`: the simulator returns loaded or generated values again

Typical use:

- test client behavior with degenerate or placeholder measurement values
- verify error handling around empty or non-informative readings

### 7.5 `SKIP <count>`

Skips the next `<count>` preloaded measurement entries from the currently loaded data set.

Example:

```powershell
hse-simulator sim SKIP 2
```

Typical use:

- continue replay at a later point in a loaded run
- align the simulator state with a partially executed workflow

### 7.6 `EXIT`

Stops the running simulator.

Example:

```powershell
hse-simulator sim EXIT
```

Typical use:

- stop the simulator from a script
- terminate a remote or background simulator session cleanly

## 8. Typical Development Workflow

1. Start the simulator with `hse-simulator evidense`.
2. Optionally preload test data with a JSON file or `LOAD`.
3. Start the client application or script with `Device("SIMULATION")`.
4. Use the web UI or `hse-simulator sim ...` commands to adjust simulator state as needed.
5. Run the normal baseline / air / sample workflow against the simulator.
