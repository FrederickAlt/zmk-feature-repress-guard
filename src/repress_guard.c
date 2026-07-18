/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#include <stddef.h>

#include <zephyr/sys/util.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/matrix.h>

#include <zmk_feature_repress_guard/repress_guard_state.h>

static struct zmk_repress_guard_state position_states[ZMK_KEYMAP_LEN];

static int repress_guard_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event =
        as_zmk_position_state_changed(eh);

    if (event == NULL || event->source != ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL ||
        event->position >= ARRAY_SIZE(position_states)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    const enum zmk_repress_guard_action action = zmk_repress_guard_update(
        &position_states[event->position], event->state, (uint32_t)event->timestamp,
        (uint32_t)CONFIG_ZMK_REPRESS_GUARD_WINDOW_MS);

    return action == ZMK_REPRESS_GUARD_FORWARD ? ZMK_EV_EVENT_BUBBLE
                                               : ZMK_EV_EVENT_HANDLED;
}

ZMK_LISTENER(repress_guard, repress_guard_listener);
ZMK_SUBSCRIPTION(repress_guard, zmk_position_state_changed);
