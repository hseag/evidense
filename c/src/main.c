// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "evibase.h"
#include "cmdget.h"
#include "cmdset.h"
#include "cmdmeasure.h"
#include "cmdbaseline.h"
#include "cmdselftest.h"
#include "cmdcommand.h"
#include "cmdfwupdate.h"
#include "cmdsave.h"
#include "cmddata.h"
#include "cmdexport.h"
#include "cmdempty.h"
#include "printerror.h"
#include <stdio.h>
#include <string.h>

#define VERSION_TOOL "0.6.0"

void help(int argcCmd, char **argvCmd)
{
	if(argvCmd == NULL)
	{
            fprintf_s(stdout, "Usage: evidense [OPTIONS] COMMAND [ARGUMENTS]\n");
            fprintf_s(stdout, "Commands:\n");
            fprintf_s(stdout, "  baseline            : starts a baseline measurement and returns the values\n");
            fprintf_s(stdout, "  command COMMAND     : executes a command e.g evidense.exe command \"V 0\" returns the value at index 0\n");
            fprintf_s(stdout, "  data                : handels data in a data file\n");
            fprintf_s(stdout, "  empty               : checks if the cuvette guide is empty\n");
            fprintf_s(stdout, "  export              : export json as csv file\n");
            fprintf_s(stdout, "  fwupdate FILE       : loads a new firmware\n");
            fprintf_s(stdout, "  get INDEX           : get a value from the device\n");
            fprintf_s(stdout, "  help COMMAND        : prints a detailed help\n");
            fprintf_s(stdout, "  levelling           : prepares the module for a measurment\n");
            fprintf_s(stdout, "  measure             : starts a measurement and return the values\n");
            fprintf_s(stdout, "  save                : save the last measurement(s)\n");            
            fprintf_s(stdout, "  selftest            : executes an internal selftest\n");
            fprintf_s(stdout, "  set INDEX VALUE     : set a value in the device\n");
            fprintf_s(stdout, "  version             : returns the version\n");            
            fprintf_s(stdout, "Options:\n");
            fprintf_s(stdout, "  --verbose           : prints debug info\n");
            fprintf_s(stdout, "  --help -h           : show this help and exit\n");
            fprintf_s(stdout, "  --device            : use the given device, if omitted the CLI searchs for a device\n");
            fprintf_s(stdout, "  --use-checksum      : use the protocol with a checksum\n");
            fprintf_s(stdout, "\n");
            fprintf_s(stdout, "The commandline tool returns the following exit codes:\n");
            fprintf_s(stdout, "    0: No error.\n");
            fprintf_s(stdout, "    1: Unknown command\n");
            fprintf_s(stdout, "    2: Invalid parameter\n");
            fprintf_s(stdout, "    3: Timeout.\n");
            fprintf_s(stdout, "    4: SREC Flash write error\n");
            fprintf_s(stdout, "    5: SREC Unsupported type\n");
            fprintf_s(stdout, "    6: SREC Invalid crc\n");
            fprintf_s(stdout, "    7: SREC Invalid string\n");
            fprintf_s(stdout, "    8: Levelling failed. Cuvette holder blocked?\n");
            fprintf_s(stdout, "   10: EviDense Module not found\n");
            fprintf_s(stdout, "   50: Unknown command line option\n");
            fprintf_s(stdout, "   51: Response error\n");
            fprintf_s(stdout, "   52: Protocol error\n");
            fprintf_s(stdout, "   53: Unknown command line argument\n");
            fprintf_s(stdout, "   55: Invalid number\n");
            fprintf_s(stdout, "   56: File not found\n");
	}
	else
	{
		if(argcCmd == 2)
		{
			if(strcmp(argvCmd[1], "get") == 0)
			{
                fprintf_s(stdout, "Usage: evidense get INDEX\n");
                fprintf_s(stdout, "  Get a value from the device\n");
                fprintf_s(stdout, "INDEX:\n");
                fprintf_s(stdout, "   0: Firmware version\n");
                fprintf_s(stdout, "   1: Serial number\n");
                fprintf_s(stdout, "   3: Production number\n");
                fprintf_s(stdout, "  10: Number of internal stored last measurements\n");
                fprintf_s(stdout, "  23: Max current in [uA] for the 230nm LED\n");
                fprintf_s(stdout, "  33: Max current in [uA] for the 260nm LED\n");
                fprintf_s(stdout, "  43: Max current in [uA] for the 280nm LED\n");
                fprintf_s(stdout, "  53: Max current in [uA] for the 340nm LED\n");
			}
			else if(strcmp(argvCmd[1], "set") == 0)
			{
                fprintf_s(stdout, "Usage: evidense set INDEX VALUE\n");
                fprintf_s(stdout, "  Set a value in the device\n");
                fprintf_s(stdout, "WARNING:\n");
                fprintf_s(stdout, "  Changing a value can damage the device or lead to incorrect results!\n");
                fprintf_s(stdout, "INDEX:\n");
                fprintf_s(stdout, "   -\n");
			}
			else if(strcmp(argvCmd[1], "save") == 0)
			{
                fprintf_s(stdout, "Usage: evidense save [OPTIONS] [FILE] [COMMENT]\n");
                fprintf_s(stdout, "  Saves the levelling data and the last measurements in the given file FILE as a JSON file.\n");
                fprintf_s(stdout, "  The optional string COMMENT is added as a comment to the measurement in the JSON file.\n");
                fprintf_s(stdout, "Options: \n");
                fprintf_s(stdout, "  --append      : append the new data at the end of the file (Default).\n");
                fprintf_s(stdout, "  --create      : create the file and add the data at the end of the file.\n");
                fprintf_s(stdout, "  --mode-raw         : append all measurments as single measurements.\n");
                fprintf_s(stdout, "  --mode-measurement : append all measurments as air-sample pairs (Default).\n");
			}
            else if(strcmp(argvCmd[1], "export") == 0)
            {
                fprintf_s(stdout, "Usage: evidense export [OPTIONS] [JSON FILE] [CSV FILE]\n");
                fprintf_s(stdout, "  Exports data from the JSON file in CSV format.\n");
                fprintf_s(stdout, "Options: \n");
                fprintf_s(stdout, "  --delimiter-comma     : use commas as separators (Default).\n");
                fprintf_s(stdout, "  --delimiter-semicolon : use semicolons as separators.\n");
                fprintf_s(stdout, "  --delimiter-tab       : use tabs as separators.\n");
                fprintf_s(stdout, "  --mode-raw            : export single measurements.\n");
                fprintf_s(stdout, "  --mode-measurement    : export air-sample pairs (Default).\n");
            }
			else if(strcmp(argvCmd[1], "data") == 0)
			{
                fprintf_s(stdout, "Usage: evidense data print FILE\n");
                fprintf_s(stdout, "  Prints the calculated values from file FILE.\n");
                fprintf_s(stdout, "Output:\n");
                fprintf_s(stdout, "  dsDNA ssDNA ssRNS purity_ratio_260/230 purity_ratio_260/280 comment\n");
                fprintf_s(stdout, "\n");
                fprintf_s(stdout, "Usage: evidense data calculate [OPTIONS] FILE\n");
                fprintf_s(stdout, "  Calculates the concentration in the given file and adds the values to the file.\n");
                fprintf_s(stdout, "  To calculate the values at least the first value must be a blank.\n");
                fprintf_s(stdout, "Options:\n");
                fprintf_s(stdout, "  --blanksStart      : number of blanks from the begining. Default is 1\n");
                fprintf_s(stdout, "  --blanksEnd        : number of blanks at the end. Default is 0\n");
                fprintf_s(stdout, "  --pathLength       : cuvette path length. Default is 1.1\n");                
			}
			else if(strcmp(argvCmd[1], "measure") == 0)
			{
                fprintf_s(stdout, "Usage: evidense measure\n");
                fprintf_s(stdout, "  Measures with all LEDs and print the values to stdout.\n");
                fprintf_s(stdout, "Usage: evidense measure LAST\n");
                fprintf_s(stdout, "  Retrives the last LAST measurement and print the values to stdout.\n");
                fprintf_s(stdout, "  The last measurement is at 0, the second last 1.\n");
                fprintf_s(stdout, "Output: all units in [uV]\n");
                fprintf_s(stdout, "  SAMPLE_230 REFERENCE_230 SAMPLE_260 REFERENCE_260 SAMPLE_280 REFERENCE_280 SAMPLE_340 REFERENCE_340\n");
			}
			else if(strcmp(argvCmd[1], "baseline") == 0)
			{
                fprintf_s(stdout, "Usage: evidense baseline\n");
                fprintf_s(stdout, "  If a levelling is needed, the command levelling is executed before a measurement is started. For this measurement, the cuvette holder must be empty.\n");
                fprintf_s(stdout, "  The firmware has an internal storage for up to ten measurements. The command baseline clears this storage.\n");
                fprintf_s(stdout, "Output: all units in [uV]\n");
                fprintf_s(stdout, "  SAMPLE_230 REFERENCE_230 SAMPLE_260 REFERENCE_260 SAMPLE_280 REFERENCE_280 SAMPLE_340 REFERENCE_340\n");
			}
			else if(strcmp(argvCmd[1], "version") == 0)
			{
                fprintf_s(stdout, "Usage: evidense version\n");
                fprintf_s(stdout, "  Prints the version of this tool to stdout.\n");
			}
			else if(strcmp(argvCmd[1], "selftest") == 0)
			{
                fprintf_s(stdout, "Usage: evidense selftest\n");
                fprintf_s(stdout, "  Executes a selftest and prints the result.\n");
                fprintf_s(stdout, "  If the result is not ok, the must common case is that the cuvette guide is blocking the optical path\n");
                fprintf_s(stdout, "  or a cuvette is stuck in the cuvette guide.\n");
			}
			else if(strcmp(argvCmd[1], "fwupdate") == 0)
			{
                fprintf_s(stdout, "Usage: evidense fwupdate SREC_FILE\n");
                fprintf_s(stdout, "  Updates the firmware.\n");
			}
            else if(strcmp(argvCmd[1], "empty") == 0)
            {
                fprintf_s(stdout, "Usage: evidense empty\n");
                fprintf_s(stdout, "  Checks if the cuvette guide is empty.\n");
                fprintf_s(stdout, "  Returns 'Empty' if the cuvette guide is empty or if not empty 'Not empty'\n");
            }
			else if(strcmp(argvCmd[1], "command") == 0)
            {
                fprintf_s(stdout, "Usage: evidense command COMMAND\n");
                fprintf_s(stdout, "  Executes any evidense command. Usefull for testing.\n");
			}
            else
			{
                fprintf_s(stdout, "No help for command '%s'\n", argvCmd[1]);
			}
		}
		else
		{
			help(0, NULL);
		}
	}
}

int main(int argc, char *argv[])
{
    Error_t ret = ERROR_EVI_OK;
	int argcCmd = argc;
	char **argvCmd = argv;
	bool options = true;
	int i = 1;
    Evi_t eviDense = {0};

	while (i < argc && options)
	{
		if (strncmp(argv[i], "--", 2) == 0 || strncmp(argv[i], "-", 1) == 0)
		{
			if (strcmp(argv[i], "--verbose") == 0)
			{
                eviDense.verbose = true;
			}
			else if (strcmp(argv[i], "--use-checksum") == 0)
			{
                eviDense.useChecksum = true;
			}
			else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				help(0, NULL);
                return ERROR_EVI_OK;
			}
			else if ((strcmp(argv[i], "--device") == 0) && (i + 1 < argc))
			{
				i++;
                eviDense.portName = argv[i];
			}
			else
			{
                return printError(ERROR_EVI_UNKOWN_COMMAND_LINE_OPTION, "Unknown option: %s\n", argv[i]);
			}
			i++;
		}
		else
		{
			options = false;
		}
	}

	argcCmd = argc - i;
	argvCmd = argv + i;

	if (argcCmd > 0)
	{
		if (strcmp(argvCmd[0], "get") == 0 && argcCmd == 2)
		{
            return cmdGet(&eviDense, argvCmd[1]);
		}
		else if (strcmp(argvCmd[0], "set") == 0 && argcCmd == 3)
		{
            return cmdSet(&eviDense, argvCmd[1], argvCmd[2]);
		}
		else if (strcmp(argvCmd[0], "measure") == 0)
		{
            return cmdMeasure(&eviDense);
		}
		else if (strcmp(argvCmd[0], "baseline") == 0)
		{
            return cmdBaseline(&eviDense);
		}
		else if (strcmp(argvCmd[0], "version") == 0)
		{
            fprintf(stdout, "%s\n", VERSION_TOOL);
		}
		else if (strcmp(argvCmd[0], "selftest") == 0)
		{
            return cmdSelftest(&eviDense);
		}
		else if (strcmp(argvCmd[0], "fwupdate") == 0 && argcCmd == 2)
		{
            return cmdFwUpdate(&eviDense, argvCmd[1]);
		}
		else if (strcmp(argvCmd[0], "command") == 0 && argcCmd == 2)
		{
            return cmdCommand(&eviDense, argvCmd[1]);
		}
		else if (strcmp(argvCmd[0], "data") == 0)
		{
            return cmdData(&eviDense, argcCmd, argvCmd);
		}
		else if (strcmp(argvCmd[0], "save") == 0)
		{
            return cmdSave(&eviDense, argcCmd, argvCmd);
		}
        else if (strcmp(argvCmd[0], "export") == 0)
        {
            return cmdExport(&eviDense, argcCmd, argvCmd);
        }
        else if (strcmp(argvCmd[0], "empty") == 0)
        {
            return cmdEmpty(&eviDense);
        }
        else if (strcmp(argvCmd[0], "help") == 0)
		{
			help(argcCmd, argvCmd);
            return ERROR_EVI_OK;
		}
		else
		{
            return printError(ERROR_EVI_UNKOWN_COMMAND_LINE_ARGUMENT, "'%s' is not a evidense command. See 'evidense --help'.", argvCmd[0]);
		}
	}
	else
	{
		help(0, NULL);
	}

	return ret;
}
