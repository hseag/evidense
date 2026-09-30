# Simulation

The eviFamily simulator supports eviDense development and workflow testing without a physical device. It accepts the same basic device protocol as the host-side interfaces when they open the device as `SIMULATION`.

## Installation

Install the simulator package from the current publication:

```bash
python -m pip install https://hseag.github.io/evidense/pre-release/simulator/dist/hse_simulator-0.2.0-py3-none-any.whl
```

## Start eviDense Simulation

Start the eviDense simulator with:

```bash
hse-simulator evidense
```

The simulator listens on TCP port `5000`. A browser-based control UI starts by default; use `--no-web` when it is not needed.

Useful variants:

```bash
hse-simulator evidense --no-web
hse-simulator --verbose evidense
hse-simulator evidense path/to/measurement-data.json
```

The optional data file preloads measurement data for repeatable development and test scenarios. Use the selected interface with the device name `SIMULATION` to connect to the running simulator.

Simulation validates the host-side and robot workflow logic, but it does not replace validation with a physical eviDense device.
