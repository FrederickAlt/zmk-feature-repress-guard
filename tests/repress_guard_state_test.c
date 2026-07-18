#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <zmk_feature_repress_guard/repress_guard_state.h>

#define WINDOW_MS 30U

static void test_normal_tap_and_rapid_repress(void) {
    struct zmk_repress_guard_state state = {0};

    assert(zmk_repress_guard_update(&state, true, 100U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, false, 120U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, true, 149U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
    assert(zmk_repress_guard_update(&state, false, 1000U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
    assert(zmk_repress_guard_update(&state, true, 1030U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
}

static void test_boundary_and_pairing(void) {
    struct zmk_repress_guard_state state = {0};

    assert(zmk_repress_guard_update(&state, false, 10U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
    assert(zmk_repress_guard_update(&state, true, 20U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, true, 21U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
    assert(zmk_repress_guard_update(&state, false, 30U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, false, 31U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
    assert(zmk_repress_guard_update(&state, true, 60U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
}

static void test_timestamp_wrap(void) {
    struct zmk_repress_guard_state state = {0};

    assert(zmk_repress_guard_update(&state, true, UINT32_MAX - 20U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, false, UINT32_MAX - 10U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_FORWARD);
    assert(zmk_repress_guard_update(&state, true, 5U, WINDOW_MS) ==
           ZMK_REPRESS_GUARD_DROP);
}

int main(void) {
    test_normal_tap_and_rapid_repress();
    test_boundary_and_pairing();
    test_timestamp_wrap();
    puts("repress_guard_state_test: all tests passed");
    return 0;
}
