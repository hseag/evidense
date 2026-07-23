// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2024 HSE AG, <opensource@hseag.com>

#pragma once

#include "evibase.h"

/**
 * @brief Executes the `empty` command to check whether the cuvette holder is empty.
 *
 * @param self Pointer to the device context queried by the command.
 * @return Error code signalling success or the type of failure encountered.
 */
Error_t cmdEmpty(Evi_t * self);

