# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2026 HSE AG, <opensource@hseag.com>

import json
import urllib.error
import urllib.parse
import urllib.request


class RestApiError(RuntimeError):
    """Raised when a REST request fails or returns a non-success status."""
    pass


class RestClient:
    """HTTP client for the eviDense REST API.

    Args:
        base_url: Base URL of the REST server.
        serial_number: Optional device serial number used for explicit
            `/devices/{device_id}/...` routes.
    """

    def __init__(self, base_url="http://127.0.0.1:8000", serial_number=None):
        self.base_url = base_url.rstrip("/")
        self.serial_number = serial_number

    def _device_path(self, suffix):
        if self.serial_number:
            device_id = urllib.parse.quote(self.serial_number)
            return f"{self.base_url}/api/v1/devices/{device_id}/{suffix}"
        return f"{self.base_url}/api/v1/device/{suffix}"

    def _run_path(self, run_id, suffix=""):
        encoded = urllib.parse.quote(run_id)
        if suffix:
            return f"{self.base_url}/api/v1/runs/{encoded}/{suffix}"
        return f"{self.base_url}/api/v1/runs/{encoded}"

    def _request_json(self, method, url, payload=None):
        data = None
        headers = {}

        if payload is not None:
            data = json.dumps(payload).encode("utf-8")
            headers["Content-Type"] = "application/json"

        request = urllib.request.Request(url, data=data, headers=headers, method=method)

        try:
            with urllib.request.urlopen(request) as response:
                return json.load(response)
        except urllib.error.HTTPError as exc:
            body = exc.read().decode("utf-8", errors="replace")
            raise RestApiError(f"HTTP {exc.code} for {url}\n{body}") from exc
        except urllib.error.URLError as exc:
            raise RestApiError(f"Request failed for {url}: {exc}") from exc

    def _request_text(self, method, url, payload=None):
        data = None
        headers = {}

        if payload is not None:
            data = json.dumps(payload).encode("utf-8")
            headers["Content-Type"] = "application/json"

        request = urllib.request.Request(url, data=data, headers=headers, method=method)

        try:
            with urllib.request.urlopen(request) as response:
                return response.read().decode("utf-8")
        except urllib.error.HTTPError as exc:
            body = exc.read().decode("utf-8", errors="replace")
            raise RestApiError(f"HTTP {exc.code} for {url}\n{body}") from exc
        except urllib.error.URLError as exc:
            raise RestApiError(f"Request failed for {url}: {exc}") from exc

    def health(self):
        """Check whether the REST service is reachable."""
        return self._request_json("GET", f"{self.base_url}/api/v1/health")

    def version(self):
        """Return the REST API and backend software version."""
        return self._request_json("GET", f"{self.base_url}/api/v1/version")

    def list_devices(self):
        """Return the devices currently visible to the REST server."""
        return self._request_json("GET", f"{self.base_url}/api/v1/devices")

    def info(self):
        """Return device information for the selected device."""
        return self._request_json("GET", self._device_path("info"))

    def selftest(self):
        """Execute a self-test on the selected device."""
        return self._request_json("POST", self._device_path("selftest"))

    def checkempty(self):
        """Check whether the cuvette holder is empty."""
        return self._request_json("GET", self._device_path("checkempty"))

    def status(self):
        """Return the service-side device status (`idle`, `busy`, or `error`)."""
        return self._request_json("GET", self._device_path("status"))

    def run_init(self, nr_of_blanks, no_purity_ratio_260_280_correction=False):
        """Initialize a new guided run.

        Args:
            nr_of_blanks: Number of blank measurements expected at the start
                of the run.
            no_purity_ratio_260_280_correction: Disable wavelength-based
                purity ratio correction when set to ``True``.

        Returns:
            The initial run snapshot including the opaque ``run_id``.
        """
        payload = {
            "nr_of_blanks": nr_of_blanks,
            "no_purity_ratio_260_280_correction": no_purity_ratio_260_280_correction,
        }
        if self.serial_number:
            payload["device_id"] = self.serial_number
        return self._request_json("POST", f"{self.base_url}/api/v1/runs", payload)

    def run_get(self, run_id):
        """Return the current snapshot of a previously created run."""
        return self._request_json("GET", self._run_path(run_id))

    def run_measure(self, run_id, comment=None):
        """Execute the next step of a run state machine.

        The first call performs the baseline step, the second call performs
        the air step, and the third call performs the sample step.
        """
        return self._request_json("POST", self._run_path(run_id, "measure"), {"comment": comment})

    def run_addkit(self, run_id, kit):
        """Import a kit into an active run."""
        return self._request_json("POST", self._run_path(run_id, "kit/import"), {"kit": kit})

    def run_export_csv(self, run_id):
        """Export the run as CSV and return the generated CSV content."""
        return self._request_text("POST", self._run_path(run_id, "export/csv"), {})

    def run_data(self, run_id):
        """Return the generated JSON data file content for a run."""
        return self._request_json("GET", self._run_path(run_id, "data"))

    def kit_create(self, data, comment=None):
        """Create a kit from JSON run data.

        Args:
            data: Full JSON data file content of a completed or partially
                completed run.
            comment: Optional comment stored in the generated kit.
        """
        payload = {
            "data": data,
            "comment": comment,
        }
        return self._request_json("POST", f"{self.base_url}/api/v1/kits", payload)
