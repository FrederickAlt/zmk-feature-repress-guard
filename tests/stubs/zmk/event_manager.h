#pragma once

typedef struct {
    void *data;
} zmk_event_t;

#define ZMK_EV_EVENT_BUBBLE 0
#define ZMK_EV_EVENT_HANDLED 1

#define ZMK_LISTENER(name, callback) extern int zmk_test_listener_##name
#define ZMK_SUBSCRIPTION(name, event_type) extern int zmk_test_subscription_##name
