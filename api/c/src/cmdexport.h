// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"

/**
 * @brief Output format options for the export command.
 */
typedef enum
{
    MODE_RAW,         /**< Export raw measurement snapshots. */
    MODE_MEASUREMENT  /**< Export processed measurement results. */
} Mode_t;

/**
 * @brief Configuration parameters used by the export command.
 */
typedef struct
{
    char delimiter;        /**< Field delimiter to use for CSV output. */
    char * filenameJson;   /**< Destination path for JSON output, or NULL to skip. */
    char * filenameCsv;    /**< Destination path for CSV output, or NULL to skip. */
    Mode_t mode;           /**< Export mode describing which dataset to emit. */
} ExportOptions_t;

/**
 * @brief Persists measurement data based on the supplied export options.
 *
 * @param options Pointer to configuration describing output files, delimiter, and mode.
 * @return Error code communicating whether the export succeeded.
 */
Error_t exportData(ExportOptions_t * options);

/**
 * @brief Implements the `export` command handler.
 *
 * @param self Pointer to the device instance that owns the measurement data.
 * @param argcCmd Number of command arguments stored in `argvCmd`.
 * @param argvCmd Array of command arguments to parse.
 * @return Error code indicating success or failure.
 */
Error_t cmdExport(Evi_t * self, int argcCmd, char **argvCmd);

