# eviDense UV C Interface

This directory contains the C-based command line interface for the eviDense UV Photometer.
It is the right entry point if you want to control the device from scripts, service tools, or an operational workflow without embedding a higher-level application API.

## Quick Start

- Main documentation: [C Command Line Interface](../../doc/c-cli.md)
- Typical use cases: self-test, baseline, measurement, run handling, JSON/CSV export
- Build context: CMake-based project for Windows and Linux

## What You Get

The C interface is exposed as a CLI executable.
It covers the core device operations needed for integration and day-to-day operation:

- inspect connected devices
- run measurements and baseline workflows
- save measurement data as JSON
- export result data as CSV
- use the tool from automation scripts
