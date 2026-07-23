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
 * @struct Quadruple_t
 * @brief Represents a set of four measurement values.
 *
 * This structure stores four measurement values typically associated with
 * absorbance readings at specific wavelengths.
 */
typedef struct
{
    double value230; /**< Measurement value at 230 nm. */
    double value260; /**< Measurement value at 260 nm. */
    double value280; /**< Measurement value at 280 nm. */
    double value340; /**< Measurement value at 340 nm. */
} Quadruple_t;

/**
 * @brief Initializes a Quadruple_t structure with all values set to the same value.
 *
 * @param value The value to assign to all four fields.
 * @return An initialized Quadruple_t structure with identical values.
 */
DLLEXPORT Quadruple_t quadruple_initAllTheSame(double value);

/**
 * @brief Initializes a Quadruple_t structure with specific values for each wavelength.
 *
 * @param value230 Measurement value at 230 nm.
 * @param value260 Measurement value at 260 nm.
 * @param value280 Measurement value at 280 nm.
 * @param value340 Measurement value at 340 nm.
 * @return An initialized Quadruple_t structure with the given values.
 */
DLLEXPORT Quadruple_t quadruple_init(double value230, double value260, double value280, double value340);

/**
 * @brief Adds two Quadruple_t structures element-wise.
 *
 * @param self Pointer to the first Quadruple_t structure.
 * @param rhs Pointer to the second Quadruple_t structure.
 * @return The result of element-wise addition.
 */
DLLEXPORT Quadruple_t quadruple_add(const Quadruple_t * self, const Quadruple_t * rhs);

/**
 * @brief Subtracts one Quadruple_t structure from another element-wise.
 *
 * @param self Pointer to the first Quadruple_t structure.
 * @param rhs Pointer to the second Quadruple_t structure.
 * @return The result of element-wise subtraction.
 */
DLLEXPORT Quadruple_t quadruple_sub(const Quadruple_t * self, const Quadruple_t * rhs);

/**
 * @brief Divides one Quadruple_t structure by another element-wise.
 *
 * @param self Pointer to the first Quadruple_t structure.
 * @param rhs Pointer to the second Quadruple_t structure.
 * @return The result of element-wise division.
 */
DLLEXPORT Quadruple_t quadruple_div(const Quadruple_t * self, const Quadruple_t * rhs);

/**
 * @brief Multiplies two Quadruple_t structures element-wise.
 *
 * @param self Pointer to the first Quadruple_t structure.
 * @param rhs Pointer to the second Quadruple_t structure.
 * @return The result of element-wise multiplication.
 */
DLLEXPORT Quadruple_t quadruple_mul(const Quadruple_t * self, const Quadruple_t * rhs);

/**
 * @brief Prints the contents of a Quadruple_t structure to the specified stream.
 *
 * @param self Pointer to the Quadruple_t structure.
 * @param stream Output file stream where the data will be printed.
 * @param newLine Whether to add a newline at the end of the output.
 */
DLLEXPORT void quadruple_print(const Quadruple_t * self, FILE * stream, bool newLine);
