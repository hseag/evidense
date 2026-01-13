// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"

/**
 * @brief Handles the `data` command for exporting or inspecting device data.
 *
 * @param self Pointer to the device instance used by the command.
 * @param argcCmd Number of command-line arguments available in `argvCmd`.
 * @param argvCmd Array of command-line arguments passed to the command.
 * @return Error code indicating success or detailed failure reason.
 */
Error_t cmdData(Evi_t * self, int argcCmd, char **argvCmd);


