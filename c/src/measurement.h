// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "singlemeasurement.h"
#include "quadruple.h"
#include <stdbool.h>
#include <cJSON.h>

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
 * @brief Computes the correction air to blank.
 *
 * @param self Pointer to the Measurement_t structure.
 * @return The correction factor as a Quadruple_t.
 */
DLLEXPORT Quadruple_t measurement_factorAbsorbanceBufferBlank(const Measurement_t * self);


/**
 * @brief Computes the dsDNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAbsorbanceBufferBlank Correction value for air to blank.
 * @param cuvettePathLength Pointer to cuvette path length. If NULL, a default value is used.
 * @return The dsDNA concentration.
 */
DLLEXPORT double measurement_dsDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength);

/**
 * @brief Computes the ssDNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAbsorbanceBufferBlank Correction value for air to blank.
 * @param cuvettePathLength Pointer to cuvette path length. If NULL, a default value is used.
 * @return The ssDNA concentration.
 */
DLLEXPORT double measurement_ssDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength);

/**
 * @brief Computes the ssRNA concentration based on the measurement.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAbsorbanceBufferBlank Correction value for air to blank.
 * @param cuvettePathLength Pointer to cuvette path length. If NULL, a default value is used.
 * @return The ssRNA concentration.
 */
DLLEXPORT double measurement_ssRNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength);

/**
 * @brief Computes the 260/230 purity ratio of the sample.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAbsorbanceBufferBlank Correction value for air to blank. 
 * @return The 260/230 purity ratio.
 */
DLLEXPORT double measurement_purityRatio260_230(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank);

/**
 * @brief Computes the 260/280 purity ratio of the sample.
 *
 * @param self Pointer to the Measurement_t structure.
 * @param factorAbsorbanceBufferBlank Correction value for air to blank.
  * @return The 260/280 purity ratio.
 */
DLLEXPORT double measurement_purityRatio260_280(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank);

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

/**
 * @brief Populates a Measurement_t structure from a JSON node.
 *
 * @param node JSON object containing the serialized measurement data.
 * @param measurement Pointer to the structure that will receive the parsed values.
 * @return true when parsing succeeds; false if required fields are missing or invalid.
 */
DLLEXPORT bool measurement_fromJson(cJSON * node, Measurement_t * measurement);

typedef struct
{
    Quadruple_t fAbsorbanceBufferBlank;
} Factors_t;

typedef struct
{
    uint32_t blanksStart;
    uint32_t blanksEnd;
    double   cuvettePathLength;

} Parameters_t;

/**
 * @brief Creates a Parameters_t structure with default initialization values.
 *
 * @return Parameters_t instance containing the default calculation parameters.
 */
DLLEXPORT Parameters_t parametersCreate();

/**
 * @brief Calculates measurement factors using the provided parameters.
 *
 * @param oMeasurments JSON array of measurement entries.
 * @param parameters Pointer to the calculation parameters.
 * @param factors Pointer to the structure that receives the computed factors.
 * @return true if the factors were computed successfully; false otherwise.
 */
DLLEXPORT bool measurement_calculateFactors(cJSON *oMeasurments, const Parameters_t * parameters, Factors_t * factors);

/**
 * @brief Calculates derived measurement values from the provided JSON data.
 *
 * @param oMeasurements JSON array containing the measurement entries to process.
 * @param parameters Pointer to the calculation parameters.
 * @return true if all derived values were computed successfully; false otherwise.
 */
DLLEXPORT bool measurement_calculate(cJSON * oMeasurements, const Parameters_t * parameters);
