# C# Interfaces

## 1. Overview

The C# package provides two different interfaces for working with the eviDense UV Photometer:

- a high-level API based on `Run`
- a low-level API based on direct `Device` access

This document serves as the C# entry point and helps you choose the right interface for your use case.

For the full generated C# API reference, see the [C# API documentation][csharp-api-docs].

## 2. Version

This documentation describes C# package version `0.12.0`.

## 3. Setup

Reference the `Hse.EviDense` assembly from your project.

The package is also available via NuGet:

- [Hseagcom.EviDense](https://www.nuget.org/packages/Hseagcom.EviDense/)

## 4. Which Interface to Use

### 4.1 C# High-Level API

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

- [C# High-Level API](./csharp-high-level.md)

### 4.2 C# Low-Level API

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

- [C# Low-Level API](./csharp-low-level.md)

## 5. Recommended Reading Order

For most users, the best order is:

1. Read this document first.
2. Continue with [C# High-Level API](./csharp-high-level.md) if you want the guided workflow.
3. Continue with [C# Low-Level API](./csharp-low-level.md) if you need direct control.

[csharp-api-docs]: https://hseag.github.io/evidense/pre-release/doc/api/csharp/api/Hse.EviDense.html
