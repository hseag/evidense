# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

import shlex
import socket
import time

import serial
import serial.tools.list_ports

from .channel import Channel
from .constants import (
    DictKeys,
    Error,
    Index,
    Selftest,
    TypeOf,
    USB,
)
from .singlemeasurement import SingleMeasurement

class LevellingChannelResult:
    def __init__(self, result, current, amplificationSample, amplificationReference):
        self.result                 = result
        self.current                = current
        self.amplificationSample    = amplificationSample
        self.amplificationReference = amplificationReference
        
    def __repr__(self):
        return "result:{} current:{} amplificationSample:{} amplificationReference:{}".format(self.result, self.current, self.amplificationSample, self.amplificationReference)
        
    def to_json(self):
        ret = {}
        ret[DictKeys.LEVELLING_RESULT]             = self.result
        ret[DictKeys.LEVELLING_CURRENT]            = self.current
        ret[DictKeys.LEVELLING_AMPLIFIERSAMPLE]    = self.amplificationSample
        ret[DictKeys.LEVELLING_AMPLIFIERREFERENCE] = self.amplificationReference
        return ret
        
class LevellingResult:
    def __init__(self, channel230, channel260, channel280, channel340):
        self.channel230 = channel230
        self.channel260 = channel260
        self.channel280 = channel280
        self.channel340 = channel340

    def __repr__(self):
    	return "230:{} 260:{} 280:{} 340:{}".format(self.channel230, self.channel260, self.channel280, self.channel340)
        
    def to_json(self):
        ret = {}
        ret[DictKeys.CH_230] = self.channel230.to_json()
        ret[DictKeys.CH_260] = self.channel260.to_json()
        ret[DictKeys.CH_280] = self.channel280.to_json()
        ret[DictKeys.CH_340] = self.channel340.to_json()
        return ret

class SelfttestResult:
    def __init__(self, result):
        self.result = result
        
    def has_problems(self):
        return self.result != 0

    def has_problems_with_iled_230(self):
        return bool(self.result & Selftest.ILED_230)
        
    def has_problems_with_iled_260(self):
        return bool(self.result & Selftest.ILED_260)
        
    def has_problems_with_iled_280(self):
        return bool(self.result & Selftest.ILED_280)

    def has_problems_with_iled_340(self):
        return bool(self.result & Selftest.ILED_340)

    def has_problems_with_sample_230(self): 
        return bool(self.result & Selftest.SAMPLE_230)
        
    def has_problems_with_sample_260(self): 
        return bool(self.result & Selftest.SAMPLE_260)
        
    def has_problems_with_sample_280(self): 
        return bool(self.result & Selftest.SAMPLE_280)
        
    def has_problems_with_sample_340(self): 
        return bool(self.result & Selftest.SAMPLE_340)
        
    def has_problems_with_reference_230(self):
        return bool(self.result & Selftest.REFERENCE_230)
        
    def has_problems_with_reference_260(self):
        return bool(self.result & Selftest.REFERENCE_260)
        
    def has_problems_with_reference_280(self):
        return bool(self.result & Selftest.REFERENCE_280)
        
    def has_problems_with_reference_340(self):
        return bool(self.result & Selftest.REFERENCE_340)

    def has_problems_with_reference(self):
        return bool(self.result & Selftest.REFERENCE)
        
    def has_problems_with_sample(self):
        return bool(self.result & Selftest.SAMPLE)
        
    def has_problems_with_split_ratio(self):
        return bool(self.result & Selftest.SPLITRATIO)
        
    def has_problems_with_levelling_230(self):
        return bool(self.result & Selftest.LEVELLING_230)
        
    def to_json(self):
        ret = {}
        ret[DictKeys.SELFTEST_RESULT] = self.result
        return ret

class Device:
    """Low-level interface to an eviDense UV Photometer."""

    def __init__(self, device = None):
        """Open a connection to a device.

        Args:
            device: Serial number, ``"SIMULATION"``, or ``None`` to auto-select
                the first matching device.
        """
        self.serial = None
        self._simulation_socket = None
        self._simulation_rx_buffer = bytearray()
        if device == "SIMULATION":
            self.is_simulation = True
            self.device = "SOCKET"
        else:
            self.is_simulation = False
            self.device = self.find_device(device)
            if self.device is None:
                raise Exception("eviDense Module not found!")
            
            self.serial = serial.Serial(self.device, 115200, timeout=30)
            self.serial.reset_input_buffer()

    def close(self):
        """Close the underlying device connection, if open."""
        if self.is_simulation:
            if self._simulation_socket is not None:
                try:
                    self._simulation_socket.close()
                finally:
                    self._simulation_socket = None
            self._simulation_rx_buffer = bytearray()
            return
        if self.serial is not None:
            try:
                self.serial.close()
            finally:
                self.serial = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc, tb):
        self.close()
        return False

    def find_device(self, device = None):
        """Find the serial port of a matching eviDense device."""
        ports = serial.tools.list_ports.comports()
        for p in ports:
            if p.vid == USB.VID and p.pid == USB.PID and (device == None or device == p.serial_number):
                 return p.device
        return

    def error2text(self, error):
        if error == Error.OK:
            return "OK"
        elif error == Error.UNKNOWN_COMMAND:
            return "Unknown command"
        elif error == Error.INVALID_PARAMETER:
            return "Invalid parameter"
        elif error == Error.TIMEOUT:
            return "Timeout"
        elif error == Error.SREC_FLASH_WRITE_ERROR:
            return "SREC Flash write error"
        elif error == Error.SREC_UNSUPPORTED_TYPE:
            return "SREC Unsupported type"
        elif error == Error.SREC_INVALID_CRC:
            return "SREC Invalid crc"
        elif error == Error.SREC_INVALID_STRING:
            return "SREC Invalid string"
        elif error == Error.LEVELLING_FAILED:
            return "levelling failed"
        else:
            return "?"

    def command(self, tx):
        if self.is_simulation:
            return self.command_socket(tx)
        else:
            return self.command_serial(tx)

    def _ensure_simulation_connection(self):
        if self._simulation_socket is not None:
            return

        self._simulation_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._simulation_socket.settimeout(30)
        self._simulation_socket.connect(("localhost", 5000))
        self._simulation_rx_buffer = bytearray()

    def _simulation_readline(self):
        while True:
            newline_index = self._simulation_rx_buffer.find(b"\n")
            if newline_index >= 0:
                line = bytes(self._simulation_rx_buffer[: newline_index + 1])
                del self._simulation_rx_buffer[: newline_index + 1]
                return line

            chunk = self._simulation_socket.recv(1024)
            if not chunk:
                return b""
            self._simulation_rx_buffer.extend(chunk)

    def command_socket(self, tx):
        self._ensure_simulation_connection()
        self._simulation_socket.sendall((":" + tx + "\n").encode())
        while True:
            rx = self._simulation_readline()
            if not rx:
                self.close()
                raise Exception("No response within time!")
            if rx[0] != (b":")[0]:
                self.close()
                raise Exception(("Response did not start with ':' {}!").format(rx))
            ba = bytearray(rx)
            if rx[1] == (b"#")[0]:
                print("DEBUG", rx)
            else:
                del ba[0]
                parts = shlex.split(ba.decode("utf-8"))
                if parts[0] == "E":
                    raise Exception("Response of TX:{} has an error: ({}) {}!".format(tx, rx, self.error2text(int(parts[1]))))
                break
        return parts

    def command_serial(self, tx):
        self.serial.write((":" + tx + "\n").encode())
        while True:
            rx = self.serial.readline()
            if rx == '':
      	        raise Exception("No response within time!")
            if rx[0] != (b':')[0] :
                raise Exception(("Response did not start with ':' {}!").format(rx))
            ba = bytearray(rx)
            if rx[1] == (b'#')[0]:
                print("DEBUG", rx)
            else:
                del ba[0]
                parts = shlex.split(ba.decode("utf-8"))
                if parts[0] == "E":
                    raise Exception("Response of TX:{} has an error: ({}) {}!".format(tx, rx, self.error2text(int(parts[1]))))
                break
        return parts

    def get(self, index):
        response = self.command(("V {}").format(index))
        typeof   = self.typeof(index)
        if typeof == TypeOf.STRING:
            return response[1]
        elif typeof == TypeOf.UINT32:
            return int(response[1])
        elif typeof == TypeOf.DOUBLE:
            return float(response[1])
        else:
            return response[1]

    def typeof(self, index):
    	response = self.command(("H {}").format(index))
    	return int(response[1])

    def serial_number(self):
        """Return the device serial number."""
        return self.get(Index.SERIALNUMBER)

    def firmware_version(self):
        """Return the firmware version string."""
        return self.get(Index.VERSION)

    def production_number(self):
        return self.get(Index.PRODUCTIONNUMBER)

    def __repr__(self):
    	return "eviDense Module@{} SN:{} Version:{}".format(self.device, self.serial_number(), self.firmware_version())

    def levelling(self):
        """Run the levelling command and return the raw levelling result."""
        response = self.command("C")
        return LevellingResult(LevellingChannelResult(int(response[1]), int(response[2]), int(response[3]), int(response[4])), 
                               LevellingChannelResult(int(response[5]), int(response[6]), int(response[7]), int(response[8])),
                               LevellingChannelResult(int(response[9]), int(response[10]), int(response[11]), int(response[12])),
                               LevellingChannelResult(int(response[13]), int(response[14]), int(response[15]), int(response[16])))
        
    def baseline(self):
        """Acquire a baseline measurement."""
        response = self.command("G")

        channel230 = Channel(int(response[1]), int(response[2]))
        channel260 = Channel(int(response[3]), int(response[4]))
        channel280 = Channel(int(response[5]), int(response[6]))
        channel340 = Channel(int(response[7]), int(response[8]))
        return SingleMeasurement(channel230, channel260, channel280, channel340)        
        
    def selftest(self):
        """Execute a self-test and return the summary result."""
        response = self.command("Y")
        return SelfttestResult(int(response[1]))
    
        
    def selftest_details_as_json(self):
        """Return detailed self-test values as a JSON-compatible dictionary."""
        def led_as_json(indexILed, indexDarkSample, indexDarkReference, indexSample, indexReference):
            ret = {}
            ret[DictKeys.SELFTEST_ILED]          = self.get(indexILed)
            ret[DictKeys.SELFTEST_DARKSAMPLE]    = self.get(indexDarkSample)
            ret[DictKeys.SELFTEST_DARKREFERENCE] = self.get(indexDarkReference)
            ret[DictKeys.SELFTEST_SAMPLE]        = self.get(indexSample)
            ret[DictKeys.SELFTEST_REFERENCE]     = self.get(indexReference)
            return ret
            
        def amplifiers_as_json():
          ret = {}
          ret[DictKeys.SELFTEST_SPLITRATION230NM]       = self.get(Index.SELFTEST_AMPLIFIER_SPLITRATIO230NM)
          ret[DictKeys.SELFTEST_AMPLIFIER_CURRENT]      = self.get(Index.SELFTEST_AMPLIFIER_CURRENT)
          ret[DictKeys.SELFTEST_AMPLIFIER_SAMPLE1]      = self.get(Index.SELFTEST_AMPLIFIER_SAMPLE1)
          ret[DictKeys.SELFTEST_AMPLIFIER_SAMPLE11]     = self.get(Index.SELFTEST_AMPLIFIER_SAMPLE11)
          ret[DictKeys.SELFTEST_AMPLIFIER_SAMPLE111]    = self.get(Index.SELFTEST_AMPLIFIER_SAMPLE111)
          ret[DictKeys.SELFTEST_AMPLIFIER_REFERENCE1]   = self.get(Index.SELFTEST_AMPLIFIER_REFERENCE1)
          ret[DictKeys.SELFTEST_AMPLIFIER_REFERENCE11]  = self.get(Index.SELFTEST_AMPLIFIER_REFERENCE11)
          ret[DictKeys.SELFTEST_AMPLIFIER_REFERENCE111] = self.get(Index.SELFTEST_AMPLIFIER_REFERENCE111)
          return ret

        ret                           = amplifiers_as_json()
        ret[DictKeys.CH_230]              = led_as_json(Index.SELFTEST_LED230_ILED, Index.SELFTEST_LED230_DARKSAMPLE, Index.SELFTEST_LED230_DARKREFERENCE, Index.SELFTEST_LED230_SAMPLE, Index.SELFTEST_LED230_REFERENCE)
        ret[DictKeys.CH_260]              = led_as_json(Index.SELFTEST_LED260_ILED, Index.SELFTEST_LED260_DARKSAMPLE, Index.SELFTEST_LED260_DARKREFERENCE, Index.SELFTEST_LED260_SAMPLE, Index.SELFTEST_LED260_REFERENCE)
        ret[DictKeys.CH_280]              = led_as_json(Index.SELFTEST_LED280_ILED, Index.SELFTEST_LED280_DARKSAMPLE, Index.SELFTEST_LED280_DARKREFERENCE, Index.SELFTEST_LED280_SAMPLE, Index.SELFTEST_LED280_REFERENCE)
        ret[DictKeys.CH_340]              = led_as_json(Index.SELFTEST_LED340_ILED, Index.SELFTEST_LED340_DARKSAMPLE, Index.SELFTEST_LED340_DARKREFERENCE, Index.SELFTEST_LED340_SAMPLE, Index.SELFTEST_LED340_REFERENCE)
        return ret

    def is_cuvette_holder_empty(self):
        """Return ``True`` if the device reports an empty cuvette holder."""
        response = self.command("X")
        return ((int)(response[1]) == 1)

    def measure(self, last = -1):
        """Acquire a measurement or retrieve a previous measurement by index."""
        command = "M"
        if last >= 0:
            command = command + (" {}").format(last)
        response = self.command(command)
              
        channel230 = Channel(int(response[1]), int(response[2]))
        channel260 = Channel(int(response[3]), int(response[4]))
        channel280 = Channel(int(response[5]), int(response[6]))
        channel340 = Channel(int(response[7]), int(response[8]))
        return SingleMeasurement(channel230, channel260, channel280, channel340)
            
    def verify(self):
        response = self.command("T")
        return ((int)(response[1]) == 1)

    def reboot(self):
        response = self.command("R")

    def erase(self):
        response = self.command("F")
        
    def fwupdate(self, filename):
        with open(filename, 'r') as file:
            self.erase()
            for srecLine in file:
                self.command("S {}".format(srecLine))
            if not self.verify():
                raise Exception("Firmware update failed. Image not valid!")
                
            self.reboot()
            
            time.sleep(30.0)
            
            #After a reboot we have to re-open the serial device
            self.serial.close()
            self.serial = serial.Serial(self.device, 115200, timeout=30)
            self.serial.reset_input_buffer()

            if self.verify():
                raise Exception("Firmware update failed. Image still valid!")
            

    def technical_report(self):
        """Return a combined technical report with levelling and self-test data."""
        result = {}
        result[DictKeys.LEVELLING]        = self.levelling().to_json()
        result[DictKeys.SELFTEST]         = self.selftest().to_json() | self.selftest_details_as_json()
        result[DictKeys.SERIALNUMBER]     = self.serial_number()
        result[DictKeys.FIRMWAREVERSION]  = self.firmware_version()
        result[DictKeys.PRODUCTIONNUMBER] = self.production_number()
        return result

    def logging(self):
        """Read and return all pending log messages from the device."""
        messages = []
        while True:
            try:
                response = self.command("Q")
                messages.append(response[1])
            except Exception:
                break
        return messages
