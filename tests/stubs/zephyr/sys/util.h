#pragma once

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define IS_ENABLED(config_macro) (config_macro == 1)
