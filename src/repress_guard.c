/*
 * Copyright (c) 2026 ZMK Repress Guard Contributors
 * SPDX-License-Identifier: MIT
 */

#include <stddef.h>

#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/matrix.h>

#include <zmk_feature_repress_guard/repress_guard_positions.h>
#include <zmk_feature_repress_guard/repress_guard_state.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static struct zmk_repress_guard_state position_states[ZMK_KEYMAP_LEN];
#endif

static int repress_guard_listener(const zmk_event_t *eh) {
#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    /*
     * The central owns keymap processing. Filtering only there avoids relying
     * on linker order relative to the peripheral transport listener, and
     * avoids applying the guard twice to remote events.
     */
    (void)eh;
    return ZMK_EV_EVENT_BUBBLE;
#else
    const struct zmk_position_state_changed *event =
        as_zmk_position_state_changed(eh);

    if (event == NULL || event->position >= ARRAY_SIZE(position_states) ||
        zmk_repress_guard_position_is_excluded(
            event->position, CONFIG_ZMK_REPRESS_GUARD_EXCLUDED_POSITIONS)) {
        if (event != NULL) {
            LOG_DBG("[DEBUG-rg41] bypass source=%u pos=%u state=%u ts=%lld",
                    event->source, event->position, event->state,
                    event->timestamp);
        }
        return ZMK_EV_EVENT_BUBBLE;
    }

    const enum zmk_repress_guard_action action = zmk_repress_guard_update(
        &position_states[event->position], event->state, (uint32_t)event->timestamp,
        (uint32_t)CONFIG_ZMK_REPRESS_GUARD_WINDOW_MS);

    LOG_DBG("[DEBUG-rg41] source=%u pos=%u state=%u ts=%lld action=%s",
            event->source, event->position, event->state, event->timestamp,
            action == ZMK_REPRESS_GUARD_FORWARD ? "forward" : "drop");

    return action == ZMK_REPRESS_GUARD_FORWARD ? ZMK_EV_EVENT_BUBBLE
                                               : ZMK_EV_EVENT_HANDLED;
#endif
}

ZMK_LISTENER(repress_guard, repress_guard_listener);
ZMK_SUBSCRIPTION(repress_guard, zmk_position_state_changed);
