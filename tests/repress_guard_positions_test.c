#include <assert.h>
#include <stdio.h>

#include <zmk_feature_repress_guard/repress_guard_positions.h>

int main(void) {
    assert(!zmk_repress_guard_position_is_excluded(12U, ""));
    assert(zmk_repress_guard_position_is_excluded(12U, "0 12 63"));
    assert(zmk_repress_guard_position_is_excluded(12U, "0,12,63"));
    assert(zmk_repress_guard_position_is_excluded(12U, "  12  "));
    assert(!zmk_repress_guard_position_is_excluded(2U, "12"));
    assert(!zmk_repress_guard_position_is_excluded(12U, "12x 13"));

    puts("repress_guard_positions_test: all tests passed");
    return 0;
}
