# Copyright (c) 2026 ZMK Repress Guard Contributors
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_REPRESS_GUARD)
  target_sources(app PRIVATE
    src/repress_guard.c
    src/repress_guard_positions.c
    src/repress_guard_state.c
  )
  target_include_directories(app PRIVATE include)
endif()
