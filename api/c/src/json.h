// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "cJSON.h"
#include "evidense.h"

/**
 * @brief Loads JSON content from a file path into a cJSON structure.
 *
 * @param file Null-terminated path to the file that should be parsed.
 * @return Pointer to the parsed cJSON document, or NULL on I/O or parse errors.
 */
DLLEXPORT cJSON *json_loadFromFile(const char *file);

/**
 * @brief Writes a cJSON document to a file path.
 *
 * @param file Null-terminated path where the JSON data should be saved.
 * @param json Pointer to the cJSON document that will be serialized.
 */
DLLEXPORT void json_saveToFile(const char* file, cJSON* json);
