/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

enum zmk_repress_guard_action {
    ZMK_REPRESS_GUARD_FORWARD = 0,
    ZMK_REPRESS_GUARD_DROP,
};

struct zmk_repress_guard_state {
    uint32_t last_release_ms;
    uint8_t flags;
};

/** Update one key position's state. */
enum zmk_repress_guard_action
zmk_repress_guard_update(struct zmk_repress_guard_state *state, bool pressed,
                         uint32_t timestamp_ms, uint32_t window_ms);
