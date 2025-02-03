// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "channel.h"
#include "quadruple.h"
#include <stdbool.h>

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/**
 * @struct SingleMeasurement_t
 * @brief Represents a single measurement with channels at specific wavelengths.
 *
 * This structure stores measurement data for four different wavelengths,
 * each represented by a Channel_t structure.
 */
typedef struct
{
    Channel_t channel230; /**< Measurement channel at 230 nm. */
    Channel_t channel260; /**< Measurement channel at 260 nm. */
    Channel_t channel280; /**< Measurement channel at 280 nm. */
    Channel_t channel340; /**< Measurement channel at 340 nm. */
} SingleMeasurement_t;

/**
 * @brief Initializes a SingleMeasurement_t structure with specified channel values.
 *
 * @param channel230 Measurement channel at 230 nm.
 * @param channel260 Measurement channel at 260 nm.
 * @param channel280 Measurement channel at 280 nm.
 * @param channel340 Measurement channel at 340 nm.
 * @return An initialized SingleMeasurement_t structure.
 */
DLLEXPORT SingleMeasurement_t singleMeasurement_init(Channel_t channel230, Channel_t channel260, Channel_t channel280, Channel_t channel340);

/**
 * @brief Retrieves the sample values from a SingleMeasurement_t structure.
 *
 * @param self Pointer to the SingleMeasurement_t structure.
 * @return A Quadruple_t structure containing the sample values for all channels.
 */
DLLEXPORT Quadruple_t singleMeasurement_sample(const SingleMeasurement_t * self);

/**
 * @brief Retrieves the reference values from a SingleMeasurement_t structure.
 *
 * @param self Pointer to the SingleMeasurement_t structure.
 * @return A Quadruple_t structure containing the reference values for all channels.
 */
DLLEXPORT Quadruple_t singleMeasurement_reference(const SingleMeasurement_t * self);

/**
 * @brief Prints the contents of a SingleMeasurement_t structure to the specified stream.
 *
 * @param self Pointer to the SingleMeasurement_t structure.
 * @param stream Output file stream where the data will be printed.
 * @param newLine Whether to add a newline at the end of the output.
 */
DLLEXPORT void singleMeasurement_print(const SingleMeasurement_t * self, FILE * stream, bool newLine);
