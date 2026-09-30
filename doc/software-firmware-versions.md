# Firmware Updates and Versions

Firmware controls the behavior of the eviDense UV Photometer device itself. In day-to-day work, firmware version information matters most before rollout, during support, and after an update.

## Current Repository Snapshot

The firmware artifact produced by this repository is:

- [firmware/evidense-0.8.2.srec](https://hseag.github.io/evidense/pre-release/firmware/evidense-0.8.2.srec){: download="evidense-0.8.2.srec" }

## What to Check in Practice

Before and after an update, confirm:

- which firmware version is installed on the device
- which firmware image is approved for the workflow
- whether the device firmware, host software, and integration assets belong to the same validated release set

## Where to Find More Detail

- Use [Update Process](software-firmware-update.md) for the product-level update flow.
- Use [Python Low-Level API](python-low-level.md), [C# Low-Level API](csharp-low-level.md), or [C CLI](c-cli.md) when you need technical access to version queries and update commands.
