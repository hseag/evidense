// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"
#include <cJSON.h>

/**
 * @brief Loads measurement JSON data from disk and optionally appends to internal buffers.
 *
 * The caller owns the returned JSON object and must release it with `cJSON_Delete`.
 *
 * @param self Pointer to the device instance receiving the loaded data.
 * @param filename Path to the JSON file to load.
 * @param append True to merge the data into the existing dataset, false to overwrite.
 * @return Newly allocated cJSON tree on success, or NULL on failure.
 */
cJSON* dataLoadJson(Evi_t* self, const char *filename, bool append);

/**
 * @brief Implements the `save` command handler.
 *
 * @param self Pointer to the device instance whose data should be persisted.
 * @param argcCmd Number of arguments received in `argvCmd`.
 * @param argvCmd Array of command arguments describing save options.
 * @return Error code indicating whether the save request succeeded.
 */
Error_t cmdSave(Evi_t * self, int argcCmd, char **argvCmd);

