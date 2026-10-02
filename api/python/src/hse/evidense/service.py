# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import json
import os
import threading
from contextlib import contextmanager

import serial.tools.list_ports

from .constants import USB
from .device import Device
from .kit import Kit
from .run import Run

_LOCKS = {}
_LOCKS_GUARD = threading.Lock()


def _resolve_path(working_dir, filename):
    if filename is None or os.path.isabs(filename):
        return filename
    return os.path.join(working_dir, filename)


def resolve_state_path(working_dir, device=None):
    return _resolve_path(working_dir, Run._resolve_state_filename(device=device))


def resolve_run_paths(working_dir, filename=None, device=None):
    working_dir = os.path.abspath(working_dir)
    data_file = _resolve_path(working_dir, filename)
    state_file = resolve_state_path(working_dir, device)
    return working_dir, data_file, state_file


def _run_state_name(value):
    if isinstance(value, Run.State):
        return value.name.lower()
    return str(value).lower()


def _read_json_if_exists(filename):
    if filename is None or not os.path.isfile(filename):
        return None
    with open(filename, "rb") as handle:
        return json.load(handle)


def _run_snapshot(run, state_file):
    data_file = getattr(run, "_filename", None)
    return {
        "state_file": state_file,
        "data_file": data_file,
        "device": "SIMULATION" if run.device.is_simulation else run.device.serial_number(),
        "nr_of_blanks": run.nr_of_blanks,
        "count": run._count,
        "next_state": _run_state_name(run._state),
        "measurement_count": len(run.storage),
        "has_factors": run._factors is not None,
    }


def _kit_payload(filename):
    return {
        "file": filename,
        "kit": Kit.load(filename).to_json(),
    }


def _lock_key(kind, value):
    normalized = os.path.abspath(value) if kind == "run" else value
    return "{}:{}".format(kind, normalized)


def _get_lock(kind, value):
    key = _lock_key(kind, value)
    with _LOCKS_GUARD:
        return _LOCKS.setdefault(key, threading.Lock())


@contextmanager
def _acquire_lock(kind, value):
    lock = _get_lock(kind, value)
    with lock:
        yield


def _device_lock_value(device=None):
    return device or "__default__"


def _device_lock_is_busy(device=None):
    lock = _get_lock("device", _device_lock_value(device))
    acquired = lock.acquire(blocking=False)
    if acquired:
        lock.release()
        return False
    return True


def _probe_device_info(device=None):
    evidense = Device(device)
    try:
        return {
            "serialnumber": evidense.serial_number(),
            "firmwareVersion": evidense.firmware_version(),
        }
    finally:
        evidense.close()


def _device_from_state_file(state_file):
    state = _read_json_if_exists(state_file)
    if state is None:
        return None
    return state.get("device")


def get_device_info(device=None):
    with _acquire_lock("device", _device_lock_value(device)):
        return _probe_device_info(device)


def list_devices():
    devices = []
    for port in serial.tools.list_ports.comports():
        if port.vid == USB.VID and port.pid == USB.PID:
            devices.append({
                "device_id": port.serial_number,
                "port": port.device,
            })
    return devices


def run_selftest(device=None, include_details=True):
    with _acquire_lock("device", _device_lock_value(device)):
        evidense = Device(device)
        try:
            payload = evidense.selftest().to_json()
            if include_details:
                payload |= evidense.selftest_details_as_json()
            payload["hasProblems"] = payload["result"] != 0
            return payload
        finally:
            evidense.close()


def check_empty(device=None):
    with _acquire_lock("device", _device_lock_value(device)):
        evidense = Device(device)
        try:
            return {
                "empty": evidense.is_cuvette_holder_empty(),
            }
        finally:
            evidense.close()


def init_run(nr_of_blanks, working_dir=".", filename=None, device=None, no_purity_ratio_260_280_correction=False):
    working_dir, data_file, state_file = resolve_run_paths(working_dir, filename, device)
    with _acquire_lock("run", state_file):
        with _acquire_lock("device", _device_lock_value(device)):
            run = Run(
                nr_of_blanks,
                path=working_dir,
                filename=data_file,
                device=device,
                no_purity_ratio_260_280_correction=no_purity_ratio_260_280_correction,
            )
            try:
                run.save_state(state_file)
                snapshot = _run_snapshot(run, state_file)
                snapshot["state"] = _read_json_if_exists(state_file)
                return snapshot
            finally:
                run.close()


def load_run(working_dir=".", filename=None, device=None):
    _, _, state_file = resolve_run_paths(working_dir, filename, device)
    return load_run_state(state_file)


def load_run_state(state_file):
    with _acquire_lock("run", state_file):
        run = Run.load_state(state_file)
        try:
            snapshot = _run_snapshot(run, state_file)
            snapshot["state"] = _read_json_if_exists(state_file)
            snapshot["data"] = _read_json_if_exists(snapshot["data_file"])
            return snapshot
        finally:
            run.close()


def measure_run(working_dir=".", filename=None, device=None, comment=None):
    _, _, state_file = resolve_run_paths(working_dir, filename, device)
    return measure_run_state(state_file, comment=comment)


def measure_run_state(state_file, comment=None):
    with _acquire_lock("run", state_file):
        device = _device_from_state_file(state_file)
        with _acquire_lock("device", _device_lock_value(device)):
            run = Run.load_state(state_file)
            try:
                run.measure(comment)
                run.save_state(state_file)
                snapshot = _run_snapshot(run, state_file)
                snapshot["state"] = _read_json_if_exists(state_file)
                snapshot["data"] = _read_json_if_exists(snapshot["data_file"])
                return snapshot
            finally:
                run.close()


def get_device_status(device=None):
    devices = list_devices()
    if device is None:
        if _device_lock_is_busy():
            return {
                "device_id": None,
                "status": "busy",
                "error": None,
            }
        for entry in devices:
            if _device_lock_is_busy(entry["device_id"]):
                return {
                    "device_id": entry["device_id"],
                    "status": "busy",
                    "error": None,
                }
        if len(devices) > 0:
            return {
                "device_id": devices[0]["device_id"],
                "status": "idle",
                "error": None,
            }
        return {
            "device_id": None,
            "status": "error",
            "error": "No available device found",
        }

    if device == "SIMULATION":
        return {
            "device_id": device,
            "status": "busy" if _device_lock_is_busy(device) else "idle",
            "error": None,
        }

    if _device_lock_is_busy(device):
        return {
            "device_id": device,
            "status": "busy",
            "error": None,
        }

    for entry in devices:
        if entry["device_id"] == device:
            return {
                "device_id": device,
                "status": "idle",
                "error": None,
            }

    return {
        "device_id": device,
        "status": "error",
        "error": f"Device '{device}' not found in available devices",
    }


def add_kit_to_run(kit_file, working_dir=".", filename=None, device=None):
    working_dir, _, state_file = resolve_run_paths(working_dir, filename, device)
    if not os.path.isabs(kit_file):
        kit_file = os.path.join(working_dir, kit_file)
    return add_kit_to_run_state(state_file, kit_file)


def add_kit_to_run_state(state_file, kit_file):
    with _acquire_lock("run", state_file):
        run = Run.load_state(state_file)
        try:
            run.import_kit(kit_file)
            run.save_state(state_file)
            snapshot = _run_snapshot(run, state_file)
            snapshot["kit_file"] = kit_file
            snapshot["state"] = _read_json_if_exists(state_file)
            return snapshot
        finally:
            run.close()


def add_kit_content_to_run_state(state_file, kit):
    with _acquire_lock("run", state_file):
        run = Run.load_state(state_file)
        try:
            run.import_kit_content(kit)
            run.save_state(state_file)
            snapshot = _run_snapshot(run, state_file)
            snapshot["kit"] = kit
            snapshot["state"] = _read_json_if_exists(state_file)
            return snapshot
        finally:
            run.close()


def export_run(working_dir=".", filename=None, device=None):
    _, _, state_file = resolve_run_paths(working_dir, filename, device)
    return export_run_state(state_file)


def export_run_state(state_file):
    with _acquire_lock("run", state_file):
        run = Run.load_state(state_file)
        try:
            run.export_as_csv()
            snapshot = _run_snapshot(run, state_file)
            data_file = snapshot["data_file"]
            csv_file = None
            if data_file is not None:
                csv_file = os.path.splitext(data_file)[0] + ".csv"
            snapshot["csv_file"] = csv_file
            return snapshot
        finally:
            run.close()


def create_kit(working_dir=".", device=None, file1=None, file2=None, comment=None):
    working_dir = os.path.abspath(working_dir)
    state_file = resolve_state_path(working_dir, device)
    with _acquire_lock("run", state_file):
        run = Run.load_state(state_file)
        try:
            if file2 is None:
                kit_file = _resolve_path(working_dir, file1)
                run.export_as_kit(kit_file, comment)
                return _kit_payload(kit_file)

            run_file = _resolve_path(working_dir, file1)
            kit = Kit.from_run(run_file, comment)
            kit_file = _resolve_path(working_dir, file2)
            kit.save(kit_file)
            return _kit_payload(kit_file)
        finally:
            run.close()
