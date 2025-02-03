// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include <stdio.h>
#include <stdbool.h>

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
    double sample;   /**< Sample value in microvolts (uV). */
    double reference; /**< Reference value in microvolts (uV). */
} Channel_t;

/**
 * @brief Initializes a Channel_t structure with given values.
 *
 * @param sample Sample value in microvolts (µV).
 * @param reference Reference value in microvolts (µV).
 * @return An initialized Channel_t structure.
 */
DLLEXPORT Channel_t channel_init(double sample, double reference);

/**
 * @brief Prints the contents of a Channel_t structure to the specified stream.
 *
 * @param self Pointer to the Channel_t structure.
 * @param stream Output file stream where the data will be printed.
 * @param newLine Whether to add a newline at the end of the output.
 */
DLLEXPORT void channel_print(const Channel_t * self, FILE * stream, bool newLine);
