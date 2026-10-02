# Simulation and Dry Runs

Integration work should start with safe checks before productive assay runs.
That usually means simulation, dry runs, or both.

The eviFamily simulator supports eviDense development and workflow testing
without a physical device. The host-side interfaces connect to the simulator by
opening the device as `SIMULATION`.

## Installation

Install the simulator package from the current publication:

```powershell
python -m pip install https://hseag.github.io/evidense/main/simulator/dist/hse_simulator-0.2.0-py3-none-any.whl
```

## Start the Simulator

Start the eviDense simulator with:

```powershell
hse-simulator evidense
```

The simulator listens on TCP port `5000`. A browser-based control UI starts by
default on port `8011`; use `--no-web` when it is not needed.

Useful variants:

```powershell
hse-simulator --no-web evidense
hse-simulator --verbose evidense
hse-simulator evidense path/to/measurement-data.json
```

The optional data file preloads measurement data for repeatable development,
regression tests, and interface validation. Use the selected interface with the
device name `SIMULATION` to connect to the running simulator.

## What to Validate

Use simulation and dry runs to validate command sequencing, result handling,
and recovery behavior before working with assay liquids. They do not replace
validation of positioning, cuvette handling, or the complete workflow with a
physical eviDense UV Photometer.

For setup details, control options, and interface examples, see
[Simulation Guide](simulation.md).
