// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evidense.h"

/**
 * @brief Handles the `run` command to execute a sequence of operations on the device.
 *
 * @param self Pointer to the device instance used throughout the command run.
 * @param argcCmd Number of command-line arguments in `argvCmd`.
 * @param argvCmd Array of command-line arguments supplied by the user.
 * @return Error code describing the command outcome.
 */
Error_t cmdRun(Evi_t * self, int argcCmd, char **argvCmd);

