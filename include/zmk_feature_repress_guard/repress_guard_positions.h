/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

bool zmk_repress_guard_position_is_excluded(uint32_t position,
                                            const char *excluded_positions);
