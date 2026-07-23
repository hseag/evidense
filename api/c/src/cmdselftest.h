// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evidense.h"

/**
 * @brief Executes the `selftest` command to validate instrument hardware.
 *
 * @param self Pointer to the device instance subjected to the self-test routine.
 * @return Error code detailing the self-test result.
 */
Error_t cmdSelftest(Evi_t * self);

