/*
 * pwm.c - PWM controller implementation
 */

#include "pwm.h"
#include "hal.h"

// Gamma correction table (gamma = 2.2)
const uint8_t gamma_table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,
    3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6,
    6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  11,  11,  11,  12,
   12,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,
   21,  21,  22,  22,  23,  24,  24,  25,  26,  26,  27,  28,  28,  29,  30,  30,
   31,  32,  33,  33,  34,  35,  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,
   44,  45,  46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,
   59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  72,  73,  74,  75,
   76,  77,  79,  80,  81,  82,  83,  85,  86,  87,  88,  90,  91,  92,  94,  95,
   96,  98,  99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 115, 116, 118,
  119, 121, 122, 124, 125, 127, 128, 130, 132, 133, 135, 136, 138, 140, 141, 143,
  145, 146, 148, 150, 151, 153, 155, 157, 158, 160, 162, 164, 165, 167, 169, 171,
  173, 174, 176, 178, 180, 182, 184, 186, 187, 189, 191, 193, 195, 197, 199, 201,
  203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231, 233,
  235, 238, 240, 242, 244, 246, 248, 250, 253, 255, 255, 255, 255, 255, 255, 255
};

void pwm_init(PwmController *pwm) {
    pwm->brightness = 0;
    pwm->direction = 1;
    pwm->pwm_period_us = 40;
}

uint8_t pwm_step(PwmController *pwm) {
    if (pwm->direction > 0) {
        if (pwm->brightness == 255) {
            pwm->direction = -1;
            pwm->brightness = 254;
        } else {
            pwm->brightness++;
        }
    } else {
        if (pwm->brightness == 0) {
            pwm->direction = 1;
            pwm->brightness = 1;
        } else {
            pwm->brightness--;
        }
    }
    return pwm->brightness;
}

uint8_t pwm_get_duty_cycle(const PwmController *pwm) {
    return gamma_table[pwm->brightness];
}

void pwm_cycle(const PwmController *pwm) {
    uint8_t duty = pwm_get_duty_cycle(pwm);
    
    // ON portion
    if (duty > 0) {
        hal_set_led(true);
        for (uint16_t i = 0; i < duty; i++) {
            hal_delay_us(pwm->pwm_period_us);
        }
    }
    
    // OFF portion
    if (duty < 255) {
        hal_set_led(false);
        for (uint16_t i = duty; i < 255; i++) {
            hal_delay_us(pwm->pwm_period_us);
        }
    }
}
