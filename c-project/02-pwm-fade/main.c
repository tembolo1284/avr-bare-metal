/*
 * PWM Fade - Software PWM on the onboard LED
 * 
 * Demonstrates hardware abstraction for testability.
 */

#define F_CPU 16000000UL

#include "hal.h"
#include "pwm.h"

int main(void) {
    hal_init();
    
    PwmController pwm;
    pwm_init(&pwm);
    
    while (1) {
        // Run 4 PWM cycles per brightness step for smoother fading
        for (uint8_t i = 0; i < 4; i++) {
            pwm_cycle(&pwm);
        }
        pwm_step(&pwm);
    }
    
    return 0;
}
