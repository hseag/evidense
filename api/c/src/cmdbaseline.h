// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"

/**
 * @brief Executes the baseline command to acquire baseline fluorescence data.
 *
 * @param self Pointer to the device instance that performs the baseline run.
 * @return Error code describing the success or failure of the baseline routine.
 */
Error_t cmdBaseline(Evi_t * self);

