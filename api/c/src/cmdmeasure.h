// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"

/**
 * @brief Executes the `measure` command to run a fluorescence measurement.
 *
 * @param self Pointer to the device instance acquiring the measurement.
 * @return Error code describing success or failure of the measurement routine.
 */
Error_t cmdMeasure(Evi_t * self);

