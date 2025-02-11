# Usage
1. Pick up a tip with your liquid handler
2. Aspirate at least 11 &#956;l of sample
3. Pick up a cuvette
4. Move the cuvette over the eviDense module
5. Execute `evidense baseline`
6. Insert the cuvette into the eviDense module
7. Execute `evidense measure`
8. Dispense approximately 10.5 &#956;l sample into the cuvette
9. Execute `evidense measure`
10. Move the cuvette out off the eviDense module
11. Execute `evidense save data.json`
12. Aspirate the sample back into the tip
13. Dispose the cuvette
14. Process the sample further according to your needs
15. Repeat steps 1-14 until all samples are processed

For a calculation of the concentration the first or last sample(s) must be blank sample(s).

With `evidense data calculate --blanksStart data.json` add the caclculated values to the data.json.

# Help
```
Usage: evidense [OPTIONS] COMMAND [ARGUMENTS]
Commands:
  baseline            : starts a baseline measurement and return the values
  command COMMAND     : executes a command e.g evidense.exe command "V 0" returns the value at index 0
  data                : handels data in a data file
  fwupdate FILE       : loads a new firmware
  get INDEX           : get a value from the device
  help COMMAND        : Prints a detailed help
  levelling           : prepares the module for a measurment
  measure             : starts a measurement and return the values
  save                : save the last measurement(s)
  export              : export json as csv file
  selftest            : executes an internal selftest
  set INDEX VALUE     : set a value in the device
  version             : return the version
  empty               : checks if the cuvette guide is empty
Options:
  --verbose           : prints debug info
  --help -h           : show this help and exit
  --device            : use the given device, if omitted the CLI searchs for a device
  --use-checksum      : use the protocol with a checksum

The commandline tool returns the following exit codes:
    0: No error.
    1: Unknown command
    2: Invalid parameter
    3: Timeout.
    4: SREC Flash write error
    5: SREC Unsupported type
    6: SREC Invalid crc
    7: SREC Invalid string
    8: Levelling failed. Cuvette holder blocked?
   10: EviDense Module not found
   50: Unknown command line option
   51: Response error
   52: Protocol error
   53: Unknown command line argument
   55: Invalid number
   56: File not found
```
# Command Details
## Command get
```
Usage: evidense get INDEX
  Get a value from the device
INDEX:
   0: Firmware version
   1: Serial number
   2: Hardware type
  10: Number of internal stored last measurements
  23: Max current in [uA] for the 230nm LED
  33: Max current in [uA] for the 260nm LED
  43: Max current in [uA] for the 280nm LED
  53: Max current in [uA] for the 340nm LED
  60: Sample amplification factor nominal 1.1
  61: Sample amplification factor nominal 11.0
  62: Sample amplification factor nominal 111.0
  63: Reference amplification factor nominal 1.1
  64: Reference amplification factor nominal 11.0
  65: Reference amplification factor nominal 111.0
  80: Levelling voltage in [uV] for the 230nm LED
  81: Levelling voltage in [uV] for the 260nm LED
  82: Levelling voltage in [uV] for the 280nm LED
  83: Levelling voltage in [uV] for the 340nm LED
```
## Command set
```
Usage: evidense set INDEX VALUE
  Set a value in the device
WARNING:
  Changing a value can damage the device or lead to incorrect results!
INDEX:
   1: Serial number
   2: Hardware type
  23: Max current in [uA] for the 230nm LED
  33: Max current in [uA] for the 260nm LED
  43: Max current in [uA] for the 280nm LED
  53: Max current in [uA] for the 340nm LED
  60: Sample amplification factor nominal 1.1
  61: Sample amplification factor nominal 11.0
  62: Sample amplification factor nominal 111.0
  63: Reference amplification factor nominal 1.1
  64: Reference amplification factor nominal 11.0
  65: Reference amplification factor nominal 111.0
  80: Levelling voltage in [uV] for the 230nm LED
  81: Levelling voltage in [uV] for the 260nm LED
  82: Levelling voltage in [uV] for the 280nm LED
  83: Levelling voltage in [uV] for the 340nm LED
```
## Command save
```
Usage: evidense save [OPTIONS] [FILE] [COMMENT]
  Saves the levelling data and the last measurements in the given file FILE as a JSON file.
  The optional string COMMENT is added as a comment to the measurement in the JSON file.
Options: 
  --append           : append the new data at the end of the file (Default).
  --create           : create the file and add the data at the end of the file.
  --mode-raw         : append all measurments as single measurements.
  --mode-measurement : append all measurments as air-sample pairs (Default).
```
## Command export
```
Usage: evidense export [OPTIONS] [JSON FILE] [CSV FILE]
  Exports data from the JSON file in CSV format.
Options: 
  --delimiter-comma     : use commas as separators (Default).
  --delimiter-semicolon : use semicolons as separators.
  --delimiter-tab       : use tabs as separators.
  --mode-raw            : export single measurements.
  --mode-measurement    : export air-sample pairs (Default).
```
## Command data
```
Usage: evidense data print FILE
  Prints the calculated values from file FILE.
Output:
  dsDNA ssDNA ssRNS purity_ratio_260/230 purity_ratio_260/280 comment


Usage: evidense data calculate [OPTIONS] FILE
  Calculates the concentration in the given file and adds the values to the file.
  To calculate the values at least the first value must be a blank.
Options:
  --blanksStart      : number of blanks from the begining. Default is 1
  --blanksEnd        : number of blanks at the end. Default is 0
```
## Command measure
```
Usage: evidense measure
  Measures with all LEDs and print the values to stdout.


Usage: evidense measure LAST
  Retrives the last LAST measurement and print the values to stdout.
  The last measurement is at 0, the second last 1.
Output: all units in [uV]
  SAMPLE_230 REFERENCE_230 SAMPLE_260 REFERENCE_260 SAMPLE_280 REFERENCE_280 SAMPLE_340 REFERENCE_340
```
## Command baseline
```
Usage: evidense baseline
  If a levelling is needed, the command levelling is executed before a measurement is started. For this measurement, the cuvette holder must be empty.
  The firmware has an internal storage for up to ten measurements. The command baseline clears this storage.
Output: all units in [uV]
  SAMPLE_230 REFERENCE_230 SAMPLE_260 REFERENCE_260 SAMPLE_280 REFERENCE_280 SAMPLE_340 REFERENCE_340
```
## Command version
```
Usage: evidense version
  Prints the version of this tool to stdout.
```
## Command selftest
```
Usage: evidense selftest
  Executes a selftest and prints the result.
  If the result is not ok, the must common case is that the cuvette guide is blocking the optical path
  or a cuvette is stuck in the cuvette guide.
```
## Command fwupdate
```
Usage: evidense fwupdate SREC_FILE
  Updates the firmware.
```
## Command empty
```
Usage: evidense empty
  Checks if the cuvette guide is empty.
  Returns 'Empty' if the cuvette guide is empty or if not empty 'Not empty'
```
## Command command
```
Usage: evidense command COMMAND
  Executes any evidense command. Usefull for testing.
