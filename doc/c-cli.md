# C Command Line Interface

## 1. Scope

This chapter documents the C command line tool only.
It does not describe a separate low-level C programming API.

## 2. Overview

The C interface is used through the command line executable.
This tool provides access to common device operations without embedding the library in application code.

General syntax:

```text
evidense-cli [OPTIONS] COMMAND [ARGUMENTS]
```

The command line tool is suitable for:

- interactive device inspection
- operational workflows
- scripting
- test and service tasks

The currently documented command set includes:

- `baseline`
- `command`
- `data`
- `fwupdate`
- `get`
- `help`
- `levelling`
- `measure`
- `run`
- `save`
- `export`
- `selftest`
- `set`
- `version`
- `empty`

## 3. Compilation with CMake

The C command line tool is built with CMake.
The command line project is intended to be compilable on both Windows and Linux.
A compiled version is available in `/c/dist`.

A typical CMake workflow is:

```text
cmake -S <source-dir> -B <build-dir>
cmake --build <build-dir>
cmake --install <build-dir>
```

The exact generator, compiler, build type, install prefix, and dependency setup depend on the target platform and project environment.

## 4. Command Syntax

Global options:

- `--verbose` prints debug information
- `--help` or `-h` prints help
- `--device` selects a specific device
- `--use-checksum` enables protocol mode with checksum

Example:

```text
evidense-cli --device SN0010 selftest
```

## 5. Main Commands

### 5.1 `info`

There is no dedicated `info` command in the current C CLI help.
Equivalent information is usually obtained through:

- `get`
- `version`
- raw access via `command`

### 5.2 `selftest`

Runs the internal self-test.

```text
evidense-cli selftest
```

If the result is not OK, a common reason is a blocked optical path or a stuck cuvette.

### 5.3 `checkempty`

The current C CLI command is named `empty`.

```text
evidense-cli empty
```

It prints:

- `Empty` if the cuvette guide is empty
- `Not empty` otherwise

### 5.4 `run`

The C CLI workflow is used through the `run` command.

`run init` also supports:

- `--no_purity_ratio_260_280_correction`
  Disables writing the device-specific 280 nm center wavelength into the run data file.
  The 260/280 purity correction is skipped.
- `--purity_ratio_260_280_correction`
  Explicitly enables wavelength-based 260/280 correction.

Typical sequence:

1. `evidense-cli run init 2`
2. `evidense-cli run measure`
3. `evidense-cli run measure`
4. `evidense-cli run measure "blank 1"`
5. `evidense-cli run measure`
6. `evidense-cli run measure`
7. `evidense-cli run measure "blank 2"`
8. `evidense-cli run measure`
9. `evidense-cli run measure`
10. `evidense-cli run measure "sample 1"`
11. `evidense-cli run measure`
12. `evidense-cli run measure`
13. `evidense-cli run measure "sample 2"`
14. `evidense-cli run export`

### 5.5 `baseline`

```text
evidense-cli baseline
```

Behavior:

- expects an empty cuvette guide
- performs levelling if required
- clears the device's internal measurement storage
- prints the baseline measurement values

### 5.6 `measure`

```text
evidense-cli measure
evidense-cli measure LAST
```

Behavior:

- starts a measurement and prints the values
- with `LAST`, retrieves a previous measurement from device history

The printed values are:

- sample and reference values for 230 nm
- sample and reference values for 260 nm
- sample and reference values for 280 nm
- sample and reference values for 340 nm

### 5.7 `save`

```text
evidense-cli save [OPTIONS] [FILE] [COMMENT]
```

This stores levelling data and the last measurements in a JSON file.

Relevant options:

- `--append`
- `--create`
- `--mode-raw`
- `--mode-measurement`

### 5.8 `export`

```text
evidense-cli export [OPTIONS] [JSON FILE] [CSV FILE]
```

This exports JSON data to CSV.

Relevant options:

- `--delimiter-comma`
- `--delimiter-semicolon`
- `--delimiter-tab`
- `--mode-raw`
- `--mode-measurement`

### 5.9 `data`

Subcommands documented in the current help:

- `evidense-cli data print FILE`
- `evidense-cli data calculate [OPTIONS] FILE`

`data calculate` adds calculated concentration values to the JSON file.

## 6. Output Formats

The C CLI uses:

- plain text output for measurement and status commands
- JSON files for persisted measurement data
- CSV files for exported measurement data

The `save` command produces JSON output files.
The `export` command produces CSV output files.

## 7. Files Produced by the Tool

Typical generated files:

- measurement JSON files created by `save`
- calculated JSON files updated by `data calculate`
- CSV files created by `export`

## 8. Typical Examples

Run a self-test:

```text
evidense-cli selftest
```

Check the cuvette guide state:

```text
evidense-cli empty
```

Query a device value:

```text
evidense-cli get 0
```

Perform a manual measurement sequence:

```text
evidense-cli run init 2
# The liquid handler aspirates at least 10 uL from the current blank or sample well.
# The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
# Start the baseline measurement for blank 1.
evidense-cli run measure
# Move the cuvette into the cuvette guide and start the air measurement for blank 1.
evidense-cli run measure
# Dispense the liquid into the cuvette and start the sample measurement for blank 1.
evidense-cli run measure "blank 1"
# Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.

# Repeat the same sequence for blank 2.
evidense-cli run measure
evidense-cli run measure
evidense-cli run measure "blank 2"

# Repeat the same sequence for sample 1.
evidense-cli run measure
evidense-cli run measure
evidense-cli run measure "sample 1"

# Repeat the same sequence for sample 2.
evidense-cli run measure
evidense-cli run measure
evidense-cli run measure "sample 2"

evidense-cli run export
```

## 9. Limitations

This chapter is intentionally limited to the command line workflow for C.
