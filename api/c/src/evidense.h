// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"
#include "commonerror.h"
#include "singlemeasurement.h"

/**
 * @struct Levelling_t
 * @brief Represents the levelling data for fluorescence measurements.
 *
 * This structure stores levelling results, current levels, and amplification values
 * for both sample and reference channels.
 */
typedef struct
{
    uint32_t result; /**< The result of the levelling process. */
    uint32_t current; /**< The current level during levelling. */
    uint32_t amplificationSample; /**< Amplification value for the sample channel. */
    uint32_t amplificationReference; /**< Amplification value for the reference channel. */
} Levelling_t;

/**
 * @enum SetupResult_t
 * @brief Represents possible results from a setup process.
 */
typedef enum
{
    SETUPRESULT_OK = 0, /**< Setup completed successfully. */
    SETUPRESULT_AMPLIFICATION_NOT_FOUND = 1, /**< Amplification level could not be determined. */
    SETUPRESULT_REFERENCE_CHANNEL_TOOH_IGH = 2, /**< Reference channel value is too high. */
    SETUPRESULT_SAMPLE_CHANNEL_TOO_HIGH = 3 /**< Sample channel value is too high. */
} SetupResult_t;

/**
 * @enum ErrorEviDense_t
 * @brief Represents error codes related to Evi dense measurements.
 */
typedef enum
{
    ERROR_EVI_LEVELLING_FAILED = ERROR_EVI_USER /**< Levelling process failed. */
} ErrorEviDense_t;

/** @name Self-Test Flags
 *  @brief Bit flags for the fluorescence self-test components.
 *  @{
 */
#define SELFTEST_ILED_230      0x00000001 /**< Self-test flag for ILED at 230 nm. */
#define SELFTEST_ILED_260      0x00000002 /**< Self-test flag for ILED at 260 nm. */
#define SELFTEST_ILED_280      0x00000004 /**< Self-test flag for ILED at 280 nm. */
#define SELFTEST_ILED_340      0x00000008 /**< Self-test flag for ILED at 340 nm. */
#define SELFTEST_SAMPLE_230    0x00000010 /**< Self-test flag for sample at 230 nm. */
#define SELFTEST_SAMPLE_260    0x00000020 /**< Self-test flag for sample at 260 nm. */
#define SELFTEST_SAMPLE_280    0x00000040 /**< Self-test flag for sample at 280 nm. */
#define SELFTEST_SAMPLE_340    0x00000080 /**< Self-test flag for sample at 340 nm. */
#define SELFTEST_REFERENCE_230 0x00000100 /**< Self-test flag for reference at 230 nm. */
#define SELFTEST_REFERENCE_260 0x00000200 /**< Self-test flag for reference at 260 nm. */
#define SELFTEST_REFERENCE_280 0x00000400 /**< Self-test flag for reference at 280 nm. */
#define SELFTEST_REFERENCE_340 0x00000800 /**< Self-test flag for reference at 340 nm. */
#define SELFTEST_REFERENCE     0x00001000 /**< Self-test flag for reference measurements. */
#define SELFTEST_SAMPLE        0x00002000 /**< Self-test flag for sample measurements. */
/** @} */

/**
 * @brief Performs a dense fluorescence measurement and stores the result.
 *
 * @param self Pointer to the Evi_t structure.
 * @param measurement Pointer to a SingleMeasurement_t structure to store the measurement result.
 * @return An error code indicating the result of the operation.
 */
DLLEXPORT Error_t eviDenseMeasure(Evi_t *self, SingleMeasurement_t * measurement);

/**
 * @brief Performs a baseline dense fluorescence measurement.
 *
 * @param self Pointer to the Evi_t structure.
 * @param measurement Pointer to a SingleMeasurement_t structure to store the baseline measurement.
 * @return An error code indicating the result of the operation.
 */
DLLEXPORT Error_t eviDenseBaseline(Evi_t *self, SingleMeasurement_t * measurement);

/**
 * @brief Performs the levelling process for dense fluorescence measurements.
 *
 * @param self Pointer to the Evi_t structure.
 * @param levelling230 Pointer to a Levelling_t structure for 230 nm levelling results.
 * @param levelling260 Pointer to a Levelling_t structure for 260 nm levelling results.
 * @param levelling280 Pointer to a Levelling_t structure for 280 nm levelling results.
 * @param levelling340 Pointer to a Levelling_t structure for 340 nm levelling results.
 * @return An error code indicating the result of the operation.
 */
DLLEXPORT Error_t eviDenseLevelling(Evi_t *self, Levelling_t *levelling230, Levelling_t *levelling260, Levelling_t *levelling280, Levelling_t *levelling340);

/**
 * @brief Retrieves the last recorded dense fluorescence measurements.
 *
 * @param self Pointer to the Evi_t structure.
 * @param last The number of previous measurements to retrieve.
 * @param measurement Pointer to a SingleMeasurement_t structure to store the measurement data.
 * @return An error code indicating the result of the operation.
 */
DLLEXPORT Error_t eviDenseLastMeasurements(Evi_t *self, uint32_t last, SingleMeasurement_t * measurement);

/**
 * @brief Retrieves the last levelling results for fluorescence measurements.
 *
 * @param self Pointer to the Evi_t structure.
 * @param levelling230 Pointer to a Levelling_t structure for the last levelling at 230 nm.
 * @param levelling260 Pointer to a Levelling_t structure for the last levelling at 260 nm.
 * @param levelling280 Pointer to a Levelling_t structure for the last levelling at 280 nm.
 * @param levelling340 Pointer to a Levelling_t structure for the last levelling at 340 nm.
 * @return An error code indicating the result of the operation.
 */
DLLEXPORT Error_t eviDenseLastLevelling(Evi_t *self, Levelling_t *levelling230, Levelling_t *levelling260, Levelling_t *levelling280, Levelling_t *levelling340);

/**
 * @brief Checks whether the cuvette holder is empty.
 *
 * @param self Pointer to the Evi_t structure.
 * @param empty Pointer to a boolean value that will be set to true if empty, false otherwise.
 * @return An error code indicating the result of the check.
 */
DLLEXPORT Error_t eviDenseIsCuvetteHolderEmpty(Evi_t * self, bool * empty);
