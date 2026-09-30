# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2026 HSE AG, <opensource@hseag.com>

import argparse
import json
import logging
import os
import sys
import traceback
from logging.handlers import RotatingFileHandler

from hse.evidense import service

def _resolve_log_file(args):
    if hasattr(args, "working_dir") and args.working_dir is not None:
        log_dir = os.path.abspath(args.working_dir)
    else:
        log_dir = os.getcwd()
    os.makedirs(log_dir, exist_ok=True)
    return os.path.join(log_dir, "evidense.log")

def _configure_logging(args):
    logger = logging.getLogger("hse.evidense")

    for handler in list(logger.handlers):
        logger.removeHandler(handler)
        handler.close()

    logger.setLevel(logging.DEBUG)

    if args.debug:
        handler = logging.StreamHandler(sys.stderr)
    else:
        handler = RotatingFileHandler(
            _resolve_log_file(args),
            maxBytes=5_000_000,
            backupCount=3,
            encoding="utf-8",
        )

    handler.setLevel(logging.DEBUG)
    handler.setFormatter(logging.Formatter("%(asctime)s %(levelname)s %(name)s: %(message)s"))
    logger.addHandler(handler)
    logger.propagate = False

def _parse_bool(value):
    if isinstance(value, bool):
        return value

    normalized = value.strip().lower()
    if normalized == "true":
        return True
    if normalized == "false":
        return False

    raise argparse.ArgumentTypeError("expected True or False")


def build_parser():
    parser = argparse.ArgumentParser(
        prog="python -m hse.evidense",
        description=(
            "Command line interface for HSE Evidense devices.\n"
            "Use the subcommands to inspect the device, run a selftest or manage measurement runs."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense --device SN0010 info\n"
            "  python -m hse.evidense --device SN0010 selftest --json\n"
            "  python -m hse.evidense --device SN0010 checkempty\n"
            "  python -m hse.evidense --device SN0010 run init 3\n"
            "  python -m hse.evidense run measure \"sample A\"\n"
            "  python -m hse.evidense kit create kit.json"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--device",
        help="Device serial number, e.g. SN0010",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Print full traceback on errors",
    )

    subparsers = parser.add_subparsers(dest="command", required=True)

    info_parser = subparsers.add_parser(
        "info",
        help="Show device information",
        description=(
            "Read basic device information from the selected device.\n"
            "The command prints the serial number and firmware version."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense info\n"
            "  python -m hse.evidense --device SN0010 info\n"
            "  python -m hse.evidense info --json"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    info_parser.add_argument(
        "--json",
        action="store_true",
        help="Print device information as JSON",
    )

    selftest_parser = subparsers.add_parser(
        "selftest",
        help="Run device selftest",
        description=(
            "Run the internal device selftest.\n"
            "Without --json the command prints a compact status line.\n"
            "With --json it prints the full selftest payload including details.\n"
            "With --file it writes the output to the given file instead of stdout."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense selftest\n"
            "  python -m hse.evidense --device SN0010 selftest\n"
            "  python -m hse.evidense selftest --json\n"
            "  python -m hse.evidense selftest --file selftest.txt"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    selftest_parser.add_argument(
        "--json",
        action="store_true",
        help="Print selftest result as JSON",
    )
    selftest_parser.add_argument(
        "--file",
        help="Write selftest output to file instead of stdout",
    )

    subparsers.add_parser(
        "checkempty",
        help="Check if the cuvette guide is empty",
        description=(
            "Check whether the cuvette guide is empty.\n"
            "The command accesses the selected device directly and exits with status 0 when empty, otherwise 1."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense checkempty\n"
            "  python -m hse.evidense --device SN0010 checkempty"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    kit_parser = subparsers.add_parser(
        "kit",
        help="Create kit files from the active run",
        description=(
            "Create kit files from the active run state.\n"
            "The command loads the persisted run state and exports its factors into a kit JSON file."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense kit create kit.json\n"
            "  python -m hse.evidense kit create kit.json --comment \"Batch A\"\n"
            "  python -m hse.evidense kit create file1.json file2.json --comment \"TODO\"\n"
            "  python -m hse.evidense --device SN0010 kit create factors.json"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    kit_parser.add_argument(
        "--working-dir",
        default=".",
        help="Working directory (default: .)",
    )
    kit_subparsers = kit_parser.add_subparsers(dest="kit_command", required=True)

    kit_create_parser = kit_subparsers.add_parser(
        "create",
        help="Create a kit from the active run",
        description=(
            "Create a kit JSON file from the active run state.\n"
            "With one file argument the command exports the current run factors into that kit file.\n"
            "With two file arguments a placeholder branch is used for later implementation."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense kit create kit.json\n"
            "  python -m hse.evidense kit create kit.json --comment \"Batch A\"\n"
            "  python -m hse.evidense kit create file1.json file2.json --comment \"TODO\""
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    kit_create_parser.add_argument(
        "file1",
        help="Primary file argument",
    )
    kit_create_parser.add_argument(
        "file2",
        nargs="?",
        help="Optional secondary file argument",
    )
    kit_create_parser.add_argument(
        "--comment",
        help="Optional kit comment",
    )

    run_parser = subparsers.add_parser(
        "run",
        help="Manage measurement runs",
        description=(
            "Manage measurement runs and their persisted state.\n"
            "A run is initialized once and then continued via the saved state file."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense run init 3\n"
            "  python -m hse.evidense run --working-dir .\\data init 3\n"
            "  python -m hse.evidense run measure \"sample A\"\n"
            "  python -m hse.evidense run addkit factors.json\n"
            "  python -m hse.evidense run export"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    run_parser.add_argument(
        "--working-dir",
        default=".",
        help="Working directory (default: .)",
    )
    run_parser.add_argument(
        "--file",
        help="Data file",
    )
    run_subparsers = run_parser.add_subparsers(dest="run_command", required=True)

    run_init_parser = run_subparsers.add_parser(
        "init",
        help="Initialize a run",
        description=(
            "Create a new run state and prepare the measurement output file.\n"
            "The argument nr_of_blanks defines how many blank measurements are required."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense run init 3\n"
            "  python -m hse.evidense run init 3 --no_purity_ratio_260_280_correction True\n"
            "  python -m hse.evidense run --file run.json init 3\n"
            "  python -m hse.evidense --device SN0010 run init 2"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    run_init_parser.add_argument(
        "nr_of_blanks",
        type=int,
        help="Number of blank measurements",
    )
    run_init_parser.add_argument(
        "--no_purity_ratio_260_280_correction",
        type=_parse_bool,
        default=False,
        help="Disable purity ratio 260/280 correction (False or True, default: False)",
    )

    run_measure_parser = run_subparsers.add_parser(
        "measure",
        help="Execute a measurement",
        description=(
            "Continue the active run by executing the next measurement step.\n"
            "The device state cycles through baseline, air and sample measurements."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense run measure\n"
            "  python -m hse.evidense run measure \"sample A\"\n"
            "  python -m hse.evidense run --working-dir .\\data measure"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    run_measure_parser.add_argument(
        "comment",
        nargs="?",
        help="Optional measurement comment",
    )

    run_subparsers.add_parser(
        "export",
        help="Export the active run JSON file as CSV",
        description=(
            "Export the current run data file as CSV.\n"
            "The command uses the active run state to locate the measurement file."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense run export\n"
            "  python -m hse.evidense run --working-dir .\\data export\n"
            "  python -m hse.evidense run --file run.json export"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    run_kit_parser = run_subparsers.add_parser(
        "addkit",
        help="Import a kit into the active run",
        description=(
            "Import factors from a kit JSON file into the active run.\n"
            "The active run state is loaded first and then updated with the imported kit."
        ),
        epilog=(
            "Examples:\n"
            "  python -m hse.evidense run addkit factors.json\n"
            "  python -m hse.evidense run --working-dir .\\data addkit kit.json\n"
            "  python -m hse.evidense run --file run.json addkit .\\kits\\factors.json"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    run_kit_parser.add_argument(
        "kit_file",
        help="Kit JSON file",
    )

    return parser


def cmd_info(args):
    result = service.get_device_info(args.device)

    if args.json:
        print(json.dumps(result, indent=2))
    else:
        print("serialnumber: {}".format(result["serialnumber"]))
        print("firmwareVersion: {}".format(result["firmwareVersion"]))

    return 0


def _write_output(text, filename=None):
    if filename is not None:
        with open(filename, "w", encoding="utf-8") as handle:
            handle.write(text)
            if not text.endswith("\n"):
                handle.write("\n")
        return

    sys.stdout.write(text)
    if not text.endswith("\n"):
        sys.stdout.write("\n")


def cmd_selftest(args):
    payload = service.run_selftest(args.device, include_details=args.json)
    has_problems = payload.pop("hasProblems")

    if args.json:
        output = json.dumps(payload, indent=2)
    else:
        output = "\n".join([
            "selftest: {}".format("FAILED" if has_problems else "OK"),
            "result: {}".format(payload["result"]),
        ])

    _write_output(output, args.file)

    return 1 if has_problems else 0


def cmd_checkempty(args):
    if service.check_empty(args.device)["empty"]:
        print("Empty")
        return 0
    else:
        print("Not empty")
        return 1

def cmd_run(args):
    if args.run_command == "init":
        service.init_run(
            args.nr_of_blanks,
            working_dir=args.working_dir,
            filename=args.file,
            device=args.device,
            no_purity_ratio_260_280_correction=args.no_purity_ratio_260_280_correction,
        )
        return 0

    if args.run_command == "measure":
        service.measure_run(
            working_dir=args.working_dir,
            filename=args.file,
            device=args.device,
            comment=args.comment,
        )
        return 0

    if args.run_command == "addkit":
        service.add_kit_to_run(
            args.kit_file,
            working_dir=args.working_dir,
            filename=args.file,
            device=args.device,
        )
        return 0

    service.export_run(
        working_dir=args.working_dir,
        filename=args.file,
        device=args.device,
    )
    return 0


def cmd_kit(args):
    if args.kit_command == "create":
        service.create_kit(
            working_dir=args.working_dir,
            device=args.device,
            file1=args.file1,
            file2=args.file2,
            comment=args.comment,
        )
        return 0

    return 1


def main(argv=None):
    parser = build_parser()
    args = parser.parse_args(argv)
    _configure_logging(args)

    try:
        if args.command == "info":
            return cmd_info(args)
        if args.command == "selftest":
            return cmd_selftest(args)
        if args.command == "checkempty":
            return cmd_checkempty(args)
        if args.command == "kit":
            return cmd_kit(args)
        return cmd_run(args)
    except Exception as exc:
        print("Error: {}".format(exc), file=sys.stderr)
        if args.debug:
            traceback.print_exc()
        return 1
