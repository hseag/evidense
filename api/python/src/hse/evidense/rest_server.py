# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import argparse
import base64
import math
import os
import sys
import logging
from logging.handlers import RotatingFileHandler
from pathlib import Path
from typing import Optional
from datetime import datetime

try:
    from fastapi import FastAPI, HTTPException
    from fastapi.responses import FileResponse
    from pydantic import BaseModel
except ImportError:
    FastAPI = None
    HTTPException = None
    FileResponse = None
    BaseModel = object

from . import service
from .kit import Kit
from .version import VERSION


def _require_rest_dependencies() -> None:
    if FastAPI is None or HTTPException is None or FileResponse is None:
        raise RuntimeError(
            "REST dependencies are not installed. Install the package with the 'rest' extra, "
            "for example: pip install 'hse-evidense[rest]'."
        )

def _resolve_log_file(args):
    if hasattr(args, "working_dir") and args.working_dir is not None:
        log_dir = os.path.abspath(args.working_dir)
    else:
        log_dir = os.getcwd()
    os.makedirs(log_dir, exist_ok=True)
    return os.path.join(log_dir, "evidense-rest.log")

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

def _encode_run_id(state_file: str) -> str:
    return base64.urlsafe_b64encode(state_file.encode("utf-8")).decode("ascii").rstrip("=")


def _decode_run_id(run_id: str, storage_dir: str) -> str:
    padding = "=" * (-len(run_id) % 4)
    try:
        decoded = base64.urlsafe_b64decode((run_id + padding).encode("ascii")).decode("utf-8")
    except Exception as exc:
        raise HTTPException(status_code=400, detail="Invalid run_id") from exc

    base_path = Path(storage_dir).resolve()
    target_path = Path(decoded).resolve()

    try:
        target_path.relative_to(base_path)
    except ValueError as exc:
        raise HTTPException(status_code=403, detail="run_id outside storage directory") from exc

    return str(target_path)


def _csv_path_from_json(data_file: Optional[str]) -> Optional[str]:
    if not data_file:
        return None
    return os.path.splitext(data_file)[0] + ".csv"


def _with_run_id(snapshot: dict) -> dict:
    payload = dict(snapshot)
    payload["run_id"] = _encode_run_id(snapshot["state_file"])
    payload.pop("state_file", None)
    payload.pop("data_file", None)
    state = payload.get("state")
    if isinstance(state, dict):
        state = dict(state)
        state.pop("filename", None)
        payload["state"] = state
    return payload


def _json_safe(value):
    if isinstance(value, float):
        if math.isnan(value) or math.isinf(value):
            return None
        return value
    if isinstance(value, dict):
        return {key: _json_safe(node) for key, node in value.items()}
    if isinstance(value, list):
        return [_json_safe(node) for node in value]
    if isinstance(value, tuple):
        return [_json_safe(node) for node in value]
    return value


def _available_devices() -> list[dict]:
    return service.list_devices()


def _resolve_single_device_id() -> Optional[str]:
    devices = _available_devices()
    if len(devices) == 0:
        raise HTTPException(status_code=404, detail="No device found")
    if len(devices) > 1:
        raise HTTPException(
            status_code=409,
            detail="Multiple devices available. Use the /devices/{device_id}/... routes.",
        )
    return devices[0]["device_id"]


def _resolve_existing_file(filename: Optional[str], detail: str) -> str:
    if filename is None or not os.path.isfile(filename):
        raise HTTPException(status_code=404, detail=detail)
    return filename


def _storage_dir(working_dir: Optional[str] = None) -> str:
    base = working_dir if working_dir is not None else "evidense-rest-data"
    path = os.path.abspath(base)
    os.makedirs(path, exist_ok=True)
    return path


def _default_kit_filename(device_id: Optional[str]) -> str:
    now = datetime.now().strftime("%Y_%m_%d_%H_%M_%S")
    suffix = device_id or "device"
    return f"evidense-{suffix}-kit-{now}.json"


class RunInitRequest(BaseModel):
    device_id: Optional[str] = None
    nr_of_blanks: int
    no_purity_ratio_260_280_correction: bool = False


class RunMeasureRequest(BaseModel):
    comment: Optional[str] = None


class RunAddKitRequest(BaseModel):
    kit: dict


class KitCreateRequest(BaseModel):
    data: dict
    comment: Optional[str] = None


def create_app(working_dir: Optional[str] = None) -> FastAPI:
    _require_rest_dependencies()
    storage_dir = _storage_dir(working_dir)
    app = FastAPI(
        title="eviDense REST API",
        version=VERSION,
        description="REST API for the eviDense UV Photometer based on the Python CLI/service layer.",
    )

    @app.get("/api/v1/health")
    def health():
        return {"status": "ok"}

    @app.get("/api/v1/version")
    def version():
        return {"apiVersion": "v1", "backendVersion": VERSION}

    @app.get("/api/v1/devices")
    def list_devices():
        return {"devices": _available_devices()}

    @app.get("/api/v1/device/info")
    def device_info():
        device_id = _resolve_single_device_id()
        return _json_safe(service.get_device_info(device_id))

    @app.post("/api/v1/device/selftest")
    def device_selftest():
        device_id = _resolve_single_device_id()
        return _json_safe(service.run_selftest(device_id, include_details=True))

    @app.get("/api/v1/device/checkempty")
    def device_checkempty():
        device_id = _resolve_single_device_id()
        return _json_safe(service.check_empty(device_id))

    @app.get("/api/v1/device/status")
    def device_status():
        return _json_safe(service.get_device_status())

    @app.get("/api/v1/devices/{device_id}/info")
    def device_info_by_id(device_id: str):
        return _json_safe(service.get_device_info(device_id))

    @app.post("/api/v1/devices/{device_id}/selftest")
    def device_selftest_by_id(device_id: str):
        return _json_safe(service.run_selftest(device_id, include_details=True))

    @app.get("/api/v1/devices/{device_id}/checkempty")
    def device_checkempty_by_id(device_id: str):
        return _json_safe(service.check_empty(device_id))

    @app.get("/api/v1/devices/{device_id}/status")
    def device_status_by_id(device_id: str):
        return _json_safe(service.get_device_status(device_id))

    @app.post("/api/v1/runs")
    def run_init(request: RunInitRequest):
        device_id = request.device_id if request.device_id is not None else _resolve_single_device_id()
        snapshot = service.init_run(
            request.nr_of_blanks,
            working_dir=storage_dir,
            filename=None,
            device=device_id,
            no_purity_ratio_260_280_correction=request.no_purity_ratio_260_280_correction,
        )
        return _json_safe(_with_run_id(snapshot))

    @app.get("/api/v1/runs/{run_id}")
    def run_get(run_id: str):
        snapshot = service.load_run_state(_decode_run_id(run_id, storage_dir))
        return _json_safe(_with_run_id(snapshot))

    @app.post("/api/v1/runs/{run_id}/measure")
    def run_measure(run_id: str, request: RunMeasureRequest):
        snapshot = service.measure_run_state(_decode_run_id(run_id, storage_dir), comment=request.comment)
        return _json_safe(_with_run_id(snapshot))

    @app.post("/api/v1/runs/{run_id}/kit/import")
    def run_addkit(run_id: str, request: RunAddKitRequest):
        snapshot = service.add_kit_content_to_run_state(_decode_run_id(run_id, storage_dir), request.kit)
        return _json_safe(_with_run_id(snapshot))

    @app.post("/api/v1/runs/{run_id}/export/csv")
    def run_export(run_id: str):
        snapshot = service.export_run_state(_decode_run_id(run_id, storage_dir))
        csv_file = _resolve_existing_file(_csv_path_from_json(snapshot.get("data_file")), "Run CSV file not found")
        return FileResponse(csv_file, media_type="text/csv", filename=Path(csv_file).name)

    @app.get("/api/v1/runs/{run_id}/data")
    def run_data(run_id: str):
        snapshot = service.load_run_state(_decode_run_id(run_id, storage_dir))
        if snapshot["data"] is None:
            raise HTTPException(status_code=404, detail="Run data file not found")
        return _json_safe(snapshot["data"])

    @app.get("/api/v1/runs/{run_id}/file/json")
    def run_json_file(run_id: str):
        snapshot = service.load_run_state(_decode_run_id(run_id, storage_dir))
        data_file = _resolve_existing_file(snapshot.get("data_file"), "Run data file not found")
        return FileResponse(data_file, media_type="application/json", filename=Path(data_file).name)

    @app.get("/api/v1/runs/{run_id}/file/csv")
    def run_csv_file(run_id: str):
        snapshot = service.load_run_state(_decode_run_id(run_id, storage_dir))
        csv_file = _resolve_existing_file(_csv_path_from_json(snapshot.get("data_file")), "Run CSV file not found")
        return FileResponse(csv_file, media_type="text/csv", filename=Path(csv_file).name)

    @app.post("/api/v1/kits")
    def create_kit(request: KitCreateRequest):
        return _json_safe(Kit.from_run_json(request.data, request.comment).to_json())

    return app

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="python -m hse.evidense.rest_server",
        description="Start the eviDense REST API server.",
    )
    parser.add_argument("--debug", action="store_true", help="Print full traceback on errors")
    parser.add_argument("--host", default="127.0.0.1", help="Bind host (default: 127.0.0.1)")
    parser.add_argument("--port", type=int, default=8000, help="Bind port (default: 8000)")
    parser.add_argument(
        "--working-dir",
        default="evidense-rest-data",
        help="Working directory for REST-generated run, CSV, and kit files (default: evidense-rest-data)",
    )
    return parser


def main(argv=None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    _configure_logging(args)
    _require_rest_dependencies()

    import uvicorn

    uvicorn.run(create_app(args.working_dir), host=args.host, port=args.port, reload=False)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
