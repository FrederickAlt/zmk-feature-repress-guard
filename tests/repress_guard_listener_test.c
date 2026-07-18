#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

/* Include the listener so the test exercises its source filtering and ordering. */
#include "../src/repress_guard.c"

static unsigned forwarded_presses;

static void dispatch(uint8_t source, uint32_t position, bool pressed,
                     int64_t timestamp) {
    struct zmk_position_state_changed position_event = {
        .source = source,
        .position = position,
        .state = pressed,
        .timestamp = timestamp,
    };
    const zmk_event_t event = {.data = &position_event};

    if (repress_guard_listener(&event) == ZMK_EV_EVENT_BUBBLE && pressed) {
        forwarded_presses++;
    }
}

int main(void) {
    /* Split-central events use a peripheral index, not SOURCE_LOCAL. */
    dispatch(0U, 12U, true, 100);
    dispatch(0U, 12U, false, 150);
    dispatch(0U, 12U, true, 200);
    dispatch(0U, 12U, false, 250);

    printf("keymap saw %u presses; expected 1\n", forwarded_presses);
    assert(forwarded_presses == 1U);

    /* Position 13 is excluded by the test's Kconfig value. */
    dispatch(0U, 13U, true, 300);
    dispatch(0U, 13U, false, 350);
    dispatch(0U, 13U, true, 400);
    dispatch(0U, 13U, false, 450);
    assert(forwarded_presses == 3U);
    return 0;
}
