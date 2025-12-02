/*
 * hal_mock.c - Mock implementation for host-based testing
 */

#include "hal.h"

static bool led_state = false;
static uint32_t led_toggles = 0;
static uint64_t total_delay_us = 0;

void hal_init(void) {
    led_state = false;
    led_toggles = 0;
    total_delay_us = 0;
}

void hal_set_led(bool on) {
    if (led_state != on) {
        led_toggles++;
    }
    led_state = on;
}

void hal_delay_us(uint16_t us) {
    total_delay_us += us;
}

void hal_mock_reset(void) {
    led_state = false;
    led_toggles = 0;
    total_delay_us = 0;
}

uint32_t hal_mock_get_led_toggles(void) {
    return led_toggles;
}

uint64_t hal_mock_get_total_delay_us(void) {
    return total_delay_us;
}

bool hal_mock_get_led_state(void) {
    return led_state;
}
