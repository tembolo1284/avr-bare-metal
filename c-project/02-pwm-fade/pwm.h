/*
 * pwm.h - PWM controller logic
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// Gamma correction lookup table (computed for gamma=2.2)
extern const uint8_t gamma_table[256];

// PWM controller state
typedef struct {
    uint8_t brightness;
    int8_t direction;
    uint16_t pwm_period_us;
} PwmController;

// Initialize a PWM controller
void pwm_init(PwmController *pwm);

// Advance brightness by one step, returns new brightness
uint8_t pwm_step(PwmController *pwm);

// Get gamma-corrected duty cycle for current brightness
uint8_t pwm_get_duty_cycle(const PwmController *pwm);

// Execute one PWM cycle (calls HAL functions)
void pwm_cycle(const PwmController *pwm);

#endif // PWM_H
