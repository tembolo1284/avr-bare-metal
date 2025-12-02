/*
 * hal.h - Hardware Abstraction Layer
 * 
 * Abstracts hardware access so we can:
 * - Run on real AVR hardware
 * - Run tests on host machine with mocks
 */

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>

// Initialize hardware (set pin directions, etc.)
void hal_init(void);

// Control the LED
void hal_set_led(bool on);

// Delay in microseconds
void hal_delay_us(uint16_t us);

// For testing: reset mock state
#ifdef TEST_BUILD
void hal_mock_reset(void);
uint32_t hal_mock_get_led_toggles(void);
uint64_t hal_mock_get_total_delay_us(void);
bool hal_mock_get_led_state(void);
#endif

#endif // HAL_H
