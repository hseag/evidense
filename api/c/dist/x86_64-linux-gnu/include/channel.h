// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <cJSON.h>

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/**
 * @struct Channel_t
 * @brief Represents a measurement channel with sample and reference values.
 *
 * This structure holds the sample and reference values of a measurement channel,
 * both measured in microvolts (uV).
 */
typedef struct
{
    uint32_t sample;   /**< Sample value in microvolts (uV). */
    uint32_t reference; /**< Reference value in microvolts (uV). */
} Channel_t;

/**
 * @brief Initializes a Channel_t structure with given values.
 *
 * @param sample Sample value in microvolts (uV).
 * @param reference Reference value in microvolts (uV).
 * @return An initialized Channel_t structure.
 */
DLLEXPORT Channel_t channel_init(uint32_t sample, uint32_t reference);

/**
 * @brief Prints the contents of a Channel_t structure to the specified stream.
 *
 * @param self Pointer to the Channel_t structure.
 * @param stream Output file stream where the data will be printed.
 * @param newLine Whether to add a newline at the end of the output.
 */
DLLEXPORT void channel_print(const Channel_t * self, FILE * stream, bool newLine);

/**
 * @brief Converts the channel data into a JSON representation.
 *
 * The caller takes ownership of the returned object and must release it using
 * `cJSON_Delete` after use.
 *
 * @param channel Pointer to the channel data to serialize.
 * @return Newly allocated cJSON object containing the serialized channel, or NULL on error.
 */
DLLEXPORT cJSON* channel_toJson(const Channel_t * channel);

/**
 * @brief Populates channel data from a JSON representation.
 *
 * @param obj Pointer to the cJSON object with `sample` and `reference` members.
 * @param channel Pointer to the channel structure that receives the parsed values.
 * @return True on success, false if validation fails or required members are missing.
 */
DLLEXPORT bool channel_fromJson(cJSON* obj, Channel_t * channel);


