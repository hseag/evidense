# Python Interfaces

## 1. Overview

### 1.1 Purpose

This document is the entry point for the Python software interfaces of the eviDense UV Photometer.
It helps you choose the right interface for your use case.

For the full generated Python API reference, see the [Python API documentation][python-api-docs].

### 1.2 Available Python Interfaces

The Python package provides four different interfaces for working with the eviDense UV Photometer:

- a high-level API based on `Run`
- a low-level API based on direct `Device` access
- a command line interface for scripting and operational use
- a Python-based REST API for integration with external software

## 2. Version

This documentation describes Python package version `0.10.0`.

## 3. Installation

To install the published wheel directly from the documentation site, use:

```bash
python -m pip install https://hseag.github.io/evidense/pre-release/api/python/dist/hse_evidense-0.10.0-py3-none-any.whl
```

Runtime dependency:

- `pyserial>=3.5`

Optional REST dependencies:

- `fastapi>=0.110`
- `uvicorn>=0.29`

After installation, the Python CLI is available as:

```bash
evidense --help
```

The package can also be started directly as a module:

```bash
python -m hse.evidense --help
```

The REST API can be started as:

```bash
python -m pip install "hse-evidense[rest] @ https://hseag.github.io/evidense/pre-release/api/python/dist/hse_evidense-0.10.0-py3-none-any.whl"
```

Then start it with:

```bash
evidense-rest --host 127.0.0.1 --port 8000
```

or

```bash
python -m hse.evidense.rest_server --host 127.0.0.1 --port 8000
```

Recommended setup:

- use a virtual environment
- verify that the device is accessible on the host system

## 4. Which Interface to Use

### 4.1 Python High-Level API

Use the high-level API if you want a guided workflow with minimal application code.

Typical use cases:

- measurement workflows driven by a liquid handler
- automatic blank handling
- automatic persistence of run data
- resumed workflows based on saved state

Main characteristics:

- built around the `Run` class
- hides most of the step-by-step device interaction
- suitable for standard operational workflows

See:

- [Python High-Level API](./python-high-level.md)

### 4.2 Python Low-Level API

Use the low-level API if you need full control over the measurement sequence.

Typical use cases:

- custom application logic
- explicit control of baseline, air, and sample steps
- direct access to device information and raw measurements
- advanced integrations that should not depend on the `Run` state machine

Main characteristics:

- built around the `Device` class and related data classes
- explicit acquisition and calculation steps
- best suited for custom integrations

See:

- [Python Low-Level API](./python-low-level.md)

### 4.3 Python Command Line Interface

Use the CLI if you want to work from the shell or integrate the software into scripts.

Typical use cases:

- operational commands
- test automation
- quick inspection of connected devices
- file-based run handling without writing Python application code

Main characteristics:

- available as `evidense` or `python -m hse.evidense`
- supports device info, self-test, empty-check, run management, and kit handling
- suitable for scripting and service workflows

See:

- [Python Command Line Interface](./python-cli.md)

### 4.4 Python REST API

Use the REST API if you want to control the software from another process or system over HTTP.

Typical use cases:

- integration with external orchestration software
- integration with liquid handler control software
- machine-to-machine access over HTTP and JSON
- service-style deployment on Windows

Main characteristics:

- implemented in Python with `FastAPI`
- exposes the Python CLI workflow through HTTP endpoints
- supports single-device and multi-device addressing
- returns structured JSON responses

See:

- [Python REST API](./python-rest.md)

## 5. Recommended Reading Order

For most users, the best order is:

1. Read this document first.
2. Continue with [Python High-Level API](./python-high-level.md) if you want the guided workflow.
3. Continue with [Python Low-Level API](./python-low-level.md) if you need direct control.
4. Use [Python Command Line Interface](./python-cli.md) for shell-based workflows.
5. Use [Python REST API](./python-rest.md) if you want HTTP-based integration.

[python-api-docs]: https://hseag.github.io/evidense/pre-release/doc/api/python/index.html
