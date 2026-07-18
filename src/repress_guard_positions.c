/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zmk_feature_repress_guard/repress_guard_positions.h>

static bool is_separator(char character) {
    return character == ' ' || character == '\t' || character == ',';
}

bool zmk_repress_guard_position_is_excluded(uint32_t position,
                                            const char *excluded_positions) {
    const char *cursor = excluded_positions;

    while (*cursor != '\0') {
        while (is_separator(*cursor)) {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        uint32_t value = 0U;
        bool has_digit = false;

        while (*cursor >= '0' && *cursor <= '9') {
            has_digit = true;
            value = value * 10U + (uint32_t)(*cursor - '0');
            cursor++;
        }

        if (has_digit && value == position &&
            (*cursor == '\0' || is_separator(*cursor))) {
            return true;
        }

        while (*cursor != '\0' && !is_separator(*cursor)) {
            cursor++;
        }
    }

    return false;
}
