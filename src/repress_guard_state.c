/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zmk_feature_repress_guard/repress_guard_state.h>

#define HAVE_PAIRED_RELEASE (1U << 0)
#define FORWARDED_PRESS_ACTIVE (1U << 1)
#define DROPPED_PRESS_ACTIVE (1U << 2)

static void remember_release(struct zmk_repress_guard_state *state,
                             uint32_t timestamp_ms) {
    state->flags |= HAVE_PAIRED_RELEASE;
    state->last_release_ms = timestamp_ms;
}

enum zmk_repress_guard_action
zmk_repress_guard_update(struct zmk_repress_guard_state *state, bool pressed,
                         uint32_t timestamp_ms, uint32_t window_ms) {
    if (pressed) {
        if ((state->flags & (FORWARDED_PRESS_ACTIVE | DROPPED_PRESS_ACTIVE)) != 0U) {
            return ZMK_REPRESS_GUARD_DROP;
        }

        if ((state->flags & HAVE_PAIRED_RELEASE) != 0U &&
            timestamp_ms - state->last_release_ms < window_ms) {
            state->flags |= DROPPED_PRESS_ACTIVE;
            return ZMK_REPRESS_GUARD_DROP;
        }

        state->flags |= FORWARDED_PRESS_ACTIVE;
        return ZMK_REPRESS_GUARD_FORWARD;
    }

    if ((state->flags & DROPPED_PRESS_ACTIVE) != 0U) {
        state->flags &= (uint8_t)~DROPPED_PRESS_ACTIVE;
        remember_release(state, timestamp_ms);
        return ZMK_REPRESS_GUARD_DROP;
    }

    if ((state->flags & FORWARDED_PRESS_ACTIVE) == 0U) {
        return ZMK_REPRESS_GUARD_DROP;
    }

    state->flags &= (uint8_t)~FORWARDED_PRESS_ACTIVE;
    remember_release(state, timestamp_ms);
    return ZMK_REPRESS_GUARD_FORWARD;
}
