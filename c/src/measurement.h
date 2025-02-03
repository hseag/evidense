// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "singlemeasurement.h"
#include "quadruple.h"
#include <stdbool.h>

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif


#define MAX_COMMENT_LENGTH 256

/**
 * @struct Measurement_t
 * @brief Represents a measurement with baseline, air, and sample values.
 *
 * This structure contains three single measurements (baseline, air, and sample) and an optional comment.
 */

typedef struct
{
    SingleMeasurement_t baseline; /**< Baseline measurement. */
    SingleMeasurement_t air;      /**< Air measurement. */
    SingleMeasurement_t sample;   /**< Sample measurement. */
    char comment[MAX_COMMENT_LENGTH]; /**< Comment associated with the measurement. */
} Measurement_t;

/**
 * @brief Initializes a Measurement_t structure with given values.
 *
 * @param baseline The baseline measurement.
 * @param air The air measurement.
 * @param sample The sample measurement.
 * @param comment A string containing a comment (null-terminated).
 * @return An initialized Measurement_t structure.
 */
DLLEXPORT Measurement_t measurement_init(SingleMeasurement_t baseline, SingleMeasurement_t air, SingleMeasurement_t sample, const char * comment);

/**
 * @brief Computes the air-to-blank correction factor for a measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @return The air-to-blank correction factor as a Quadruple_t.
 */
DLLEXPORT Quadruple_t measurement_factorAirToBlank(const Measurement_t * self);

/**
 * @brief Computes the correction factor from 340 nm to another wavelength.
 *
 * @param self Pointer to the Measurement_t structure.
 * @return The correction factor as a Quadruple_t.
 */
DLLEXPORT Quadruple_t measurement_factor340ToNNN(const Measurement_t * self);

/**
 * @brief Computes the dsDNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAirToBlank Correction factor for air to blank.
 * @param factor340ToNNN Correction factor from 340 nm to another wavelength.
 * @return The dsDNA concentration.
 */
DLLEXPORT double measurement_dsDNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN);

/**
 * @brief Computes the ssDNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAirToBlank Correction factor for air to blank.
 * @param factor340ToNNN Correction factor from 340 nm to another wavelength.
 * @return The ssDNA concentration.
 */
DLLEXPORT double measurement_ssDNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN);

/**
 * @brief Computes the ssRNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAirToBlank Correction factor for air to blank.
 * @param factor340ToNNN Correction factor from 340 nm to another wavelength.
 * @return The ssRNA concentration.
 */
DLLEXPORT double measurement_ssRNA(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN);

/**
 * @brief Computes the 260/230 purity ratio of the sample.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAirToBlank Correction factor for air to blank.
 * @param factor340ToNNN Correction factor from 340 nm to another wavelength.
 * @return The 260/230 purity ratio.
 */
DLLEXPORT double measurement_purityRatio260_230(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN);

/**
 * @brief Computes the 260/280 purity ratio of the sample.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAirToBlank Correction factor for air to blank.
 * @param factor340ToNNN Correction factor from 340 nm to another wavelength.
 * @return The 260/280 purity ratio.
 */
DLLEXPORT double measurement_purityRatio260_280(const Measurement_t * self, const Quadruple_t * factorAirToBlank, const Quadruple_t * factor340ToNNN);

/**
 * @brief Prints the contents of a Measurement_t structure to the specified stream.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param stream Output file stream where the data will be printed.
 * @param newLine Whether to add a newline at the end of the output.
 */
DLLEXPORT void measurement_print(const Measurement_t * self, FILE * stream, bool newLine);

/**
 * @brief Calculates the absorbance value based on a given baseline and measurement.
 *
 * @param baseline Pointer to the baseline measurement.
 * @param measurement Pointer to the actual measurement.
 * @param correctionFactor Pointer to the correction factor applied to the measurement.
 * @return The computed absorbance value as a Quadruple_t.
 */
DLLEXPORT Quadruple_t measurement_calculateAbsorbance(const SingleMeasurement_t * baseline, const SingleMeasurement_t * measurement, const Quadruple_t * correctionFactor);
