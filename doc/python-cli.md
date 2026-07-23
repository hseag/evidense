# Python Command Line Interface

## 1. Scope

This chapter documents the Python command line interface.
It complements the low-level Python API documentation.

## 2. Overview

The Python CLI provides direct access to common device operations from the command line.
Its implementation is exposed in the generated API docs through the [`hse.evidense.cli`][cli-module-api] module.

Entry points:

```bash
evidense --help
python -m hse.evidense --help
```

## 3. Command Syntax

General syntax:

```bash
python -m hse.evidense [--device SERIAL] [--debug] COMMAND [OPTIONS]
```

Global options:

- `--device SN0010` selects a specific device
- `--debug` prints a full traceback on failure

The currently implemented top-level commands are parsed in [`main()`][cli-main-api]. The current top-level commands are:

- `info`
- `selftest`
- `checkempty`
- `run`
- `kit`

## 4. Main Commands

### 4.1 `info`

Reads basic device information and prints:

- serial number
- firmware version

Examples:

```bash
python -m hse.evidense info
python -m hse.evidense --device SN0010 info
python -m hse.evidense info --json
```

### 4.2 `selftest`

Runs the internal device self-test.

Behavior:

- plain text output by default
- JSON output with `--json`
- optional file output with `--file`
- process return code indicates success or failure of the self-test

Examples:

```bash
python -m hse.evidense selftest
python -m hse.evidense selftest --json
python -m hse.evidense selftest --file selftest.txt
```

### 4.3 `checkempty`

Checks whether the cuvette guide is empty.

Current behavior:

- prints `Empty` if empty
- prints `Not empty` otherwise
- exits with status `0` when empty and `1` otherwise

Example:

```bash
python -m hse.evidense checkempty
```

### 4.4 `run`

Manages persisted run state.

Subcommands:

- `init NR_OF_BLANKS`
- `measure [COMMENT]`
- `addkit KIT_FILE`
- `export`

Key options:

- `--working-dir`
- `--file`
- `--no_purity_ratio_260_280_correction True|False` on `run init`

Examples:

```bash
python -m hse.evidense run init 2
# The liquid handler aspirates at least 10 uL from the current blank or sample well.
# The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
# Start the baseline measurement for blank 1.
python -m hse.evidense run measure
# Move the cuvette into the cuvette guide and start the air measurement for blank 1.
python -m hse.evidense run measure
# Dispense the liquid into the cuvette and start the sample measurement for blank 1.
python -m hse.evidense run measure "blank 1"
# Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.

# Repeat the same sequence for blank 2.
python -m hse.evidense run measure
python -m hse.evidense run measure
python -m hse.evidense run measure "blank 2"

# Repeat the same sequence for sample 1.
python -m hse.evidense run measure
python -m hse.evidense run measure
python -m hse.evidense run measure "sample 1"

# Repeat the same sequence for sample 2.
python -m hse.evidense run measure
python -m hse.evidense run measure
python -m hse.evidense run measure "sample 2"
python -m hse.evidense run addkit factors.json
python -m hse.evidense run export
```

`run` uses a saved state file so the workflow can continue across invocations.

### 4.5 `kit`

Creates kit files from the active run state.

Currently documented subcommand:

- `kit create`

Examples:

```bash
python -m hse.evidense kit create kit.json
python -m hse.evidense kit create kit.json --comment "Batch A"
```

## 5. Output Formats

The Python CLI supports:

- plain text output to stdout
- JSON output for commands that offer `--json`
- file output for commands such as `selftest --file`
- persisted JSON and CSV files through the `run` workflow

Logging behavior:

- with `--debug`, errors are shown on stderr with traceback
- otherwise logs are written to `evidense.log` in the working directory

For `run` and `kit`, the working directory defaults to the current directory.

## 6. Typical Examples

Query device information:

```bash
python -m hse.evidense --device SN0010 info
```

Run a self-test and print JSON:

```bash
python -m hse.evidense --device SN0010 selftest --json
```

Check whether the cuvette guide is empty:

```bash
python -m hse.evidense --device SN0010 checkempty
```

Execute the same blank-and-sample workflow via the CLI:

```bash
python -m hse.evidense run --working-dir .\data init 2
# The liquid handler aspirates at least 10 uL from the current blank or sample well.
# The liquid handler picks up a cuvette with the tip and moves above the cuvette guide.
# Start the baseline measurement for blank 1.
python -m hse.evidense run --working-dir .\data measure
# Move the cuvette into the cuvette guide and start the air measurement for blank 1.
python -m hse.evidense run --working-dir .\data measure
# Dispense the liquid into the cuvette and start the sample measurement for blank 1.
python -m hse.evidense run --working-dir .\data measure "blank 1"
# Aspirate the liquid back into the tip, leave the cuvette guide, and discard tip plus cuvette.

# Repeat the same sequence for blank 2.
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure "blank 2"

# Repeat the same sequence for sample 1.
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure "sample 1"

# Repeat the same sequence for sample 2.
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure
python -m hse.evidense run --working-dir .\data measure "sample 2"
python -m hse.evidense run --working-dir .\data export
```

## 7. Relationship to the Low-Level API

The CLI is useful for scripting and operational workflows.
For application integration and explicit measurement handling, use the low-level Python API described in [Python Low-Level API](./python-low-level.md).

[cli-module-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.cli.html
[cli-main-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.cli.html#hse.evidense.cli.main
[main-module-api]: https://hseag.github.io/evidense/pre-release/doc/api/python/api/hse.evidense.__main__.html
