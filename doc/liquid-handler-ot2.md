# Opentrons OT-2 Liquid Handler Integration

## 1. Introduction

This document describes a practical starting point for integrating the eviDense
UV Python software with an Opentrons OT-2 liquid handler.

## 2. Installation

### 2.1 Software Setup

1. [SSH](https://support.opentrons.com/en/articles/3287453-connecting-to-your-ot-2-with-ssh) into the OT-2.
2. Install the Python package with:

```bash
python -m pip install https://hseag.github.io/evidense/pre-release/api/python/dist/hse_evidense-0.10.0-py3-none-any.whl --no-deps
```

If the OT-2 has no internet connection, download the Python wheel to your computer:

[`hse_evidense-0.10.0-py3-none-any.whl`](https://hseag.github.io/evidense/pre-release/api/python/dist/hse_evidense-0.10.0-py3-none-any.whl){ download="hse_evidense-0.10.0-py3-none-any.whl" }

and copy it to your OT-2:

```bash
scp -O -i ot2_ssh_key hse_evidense-0.10.0-py3-none-any.whl root@YOUR_IP:
```

Then install it locally on the OT-2 with:

```bash
python -m pip install hse_evidense-0.10.0-py3-none-any.whl --no-deps
```

After the installation, restart the OT-2.

### 2.2 Hardware Setup

1. Connect the eviDense UV to its power supply and to the OT-2 with the USB cable.
2. Prepare the deck, labware, source liquids, and cuvettes as specified in the user guide for the selected protocol.
3. Wait until the eviDense UV power-on self-test is complete and the instrument is ready.

### 2.3 Labware

Download and install the custom eviDense labware definition in the Opentrons
App before loading a protocol:

- [`hse_evidense_pilot_right_20ul_tip_v3.json`](https://hseag.github.io/evidense/pre-release/integration_kits/opentrons-ot2/labware/hse_evidense_pilot_right_20ul_tip_v3.json){ download="hse_evidense_pilot_right_20ul_tip_v3.json" }

### 2.4 Protocols and User Guides

Install the selected protocol in the Opentrons App. The protocol-specific user
guide is the authoritative source for its deck layout, run parameters, liquid
volumes, and operating procedure. Only public protocols are listed below;
internal protocols are not part of this user documentation.

- [`evidense_ot2_device_demo.py`](https://hseag.github.io/evidense/pre-release/integration_kits/opentrons-ot2/protocol/evidense_ot2_device_demo.py){ download="evidense_ot2_device_demo.py" }: Demonstrates eviDense device control, from sample aspiration and cuvette handling to measurement and liquid disposal. It is an integration demonstration, not a validated assay workflow. See the [Device Demo user guide](evidense_ot2_device_demo_user_guide.md).

## 3. Starting The Protocol

Before running a protocol for the first time, perform the [Labware Position Check](https://docs.opentrons.com/ot-2/calibration/labware-offsets/) and simulate the selected protocol in the Opentrons App.

For a real run, follow the selected protocol's user guide. After the run, review the OT-2 run log and the eviFluor Duo Fluorometer-exported results.