/*
 * test_pwm.c - Unit tests for PWM controller
 * 
 * Compile and run on host machine (not AVR).
 * Uses mock HAL implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../pwm.h"
#include "../hal.h"

// Test counters
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("  %-50s", #name); \
    hal_mock_reset(); \
    test_##name(); \
    tests_run++; \
    tests_passed++; \
    printf("PASS\n"); \
} while(0)

// ============================================================================
// Gamma Table Tests
// ============================================================================

TEST(gamma_table_boundaries) {
    assert(gamma_table[0] == 0);
    assert(gamma_table[255] == 255);
}

TEST(gamma_table_monotonic) {
    uint8_t prev = 0;
    for (int i = 0; i < 256; i++) {
        assert(gamma_table[i] >= prev);
        prev = gamma_table[i];
    }
}

TEST(gamma_correction_dims_low_values) {
    // At 50% input, output should be much less due to gamma curve
    assert(gamma_table[128] < 100);
}

TEST(gamma_table_midpoint_sanity) {
    // Gamma 2.2: (0.5)^2.2 * 255 ≈ 56
    // Our table should be in that ballpark
    assert(gamma_table[128] > 40 && gamma_table[128] < 80);
}

// ============================================================================
// PWM Controller State Tests
// ============================================================================

TEST(pwm_init_starts_at_zero) {
    PwmController pwm;
    pwm_init(&pwm);
    
    assert(pwm.brightness == 0);
    assert(pwm.direction == 1);
    assert(pwm.pwm_period_us == 40);
}

TEST(pwm_step_increments) {
    PwmController pwm;
    pwm_init(&pwm);
    
    pwm_step(&pwm);
    assert(pwm.brightness == 1);
    
    pwm_step(&pwm);
    assert(pwm.brightness == 2);
}

TEST(pwm_step_reverses_at_max) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 255;
    pwm.direction = 1;
    
    pwm_step(&pwm);
    
    assert(pwm.direction == -1);
    assert(pwm.brightness == 254);
}

TEST(pwm_step_reverses_at_min) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 0;
    pwm.direction = -1;
    
    pwm_step(&pwm);
    
    assert(pwm.direction == 1);
    assert(pwm.brightness == 1);
}

TEST(pwm_full_cycle_up) {
    PwmController pwm;
    pwm_init(&pwm);
    
    // Step all the way up
    while (pwm.brightness < 255) {
        pwm_step(&pwm);
    }
    
    assert(pwm.brightness == 255);
    assert(pwm.direction == 1);  // Not reversed yet
    
    // One more step should reverse
    pwm_step(&pwm);
    assert(pwm.direction == -1);
}

TEST(pwm_full_cycle_down) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 255;
    pwm.direction = -1;
    
    // Step all the way down
    while (pwm.brightness > 0) {
        pwm_step(&pwm);
    }
    
    assert(pwm.brightness == 0);
    
    // One more step should reverse
    pwm_step(&pwm);
    assert(pwm.direction == 1);
}

// ============================================================================
// PWM Duty Cycle Tests
// ============================================================================

TEST(pwm_duty_cycle_uses_gamma) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 128;
    
    uint8_t duty = pwm_get_duty_cycle(&pwm);
    assert(duty == gamma_table[128]);
}

TEST(pwm_duty_cycle_at_zero) {
    PwmController pwm;
    pwm_init(&pwm);
    
    uint8_t duty = pwm_get_duty_cycle(&pwm);
    assert(duty == 0);
}

TEST(pwm_duty_cycle_at_max) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 255;
    
    uint8_t duty = pwm_get_duty_cycle(&pwm);
    assert(duty == 255);
}

// ============================================================================
// PWM Cycle Hardware Interaction Tests
// ============================================================================

TEST(pwm_cycle_correct_total_timing) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 128;
    
    pwm_cycle(&pwm);
    
    // Total should always be 255 * 40µs = 10200µs
    assert(hal_mock_get_total_delay_us() == 255 * 40);
}

TEST(pwm_cycle_zero_brightness_led_off) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 0;
    
    pwm_cycle(&pwm);
    
    // LED should never turn on (duty cycle is 0)
    assert(hal_mock_get_led_state() == false);
    assert(hal_mock_get_led_toggles() == 0);
}

TEST(pwm_cycle_max_brightness_led_on) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 255;
    
    pwm_cycle(&pwm);
    
    // LED should be on the whole time (duty cycle is 255)
    assert(hal_mock_get_led_state() == true);
}

TEST(pwm_cycle_mid_brightness_toggles) {
    PwmController pwm;
    pwm_init(&pwm);
    pwm.brightness = 128;
    
    pwm_cycle(&pwm);
    
    // Should toggle on, then off
    assert(hal_mock_get_led_toggles() == 2);
    assert(hal_mock_get_led_state() == false);  // Ends in off state
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(full_fade_cycle_timing_consistent) {
    PwmController pwm;
    pwm_init(&pwm);
    
    // Run 10 brightness steps
    for (int step = 0; step < 10; step++) {
        hal_mock_reset();
        
        pwm_cycle(&pwm);
        
        // Every cycle should have the same total timing
        assert(hal_mock_get_total_delay_us() == 255 * 40);
        
        pwm_step(&pwm);
    }
}

// ============================================================================
// Main
// ============================================================================

int main(void) {
    printf("\n");
    printf("=================================================\n");
    printf("  PWM Controller Unit Tests\n");
    printf("=================================================\n\n");
    
    printf("Gamma Table:\n");
    RUN_TEST(gamma_table_boundaries);
    RUN_TEST(gamma_table_monotonic);
    RUN_TEST(gamma_correction_dims_low_values);
    RUN_TEST(gamma_table_midpoint_sanity);
    
    printf("\nPWM Controller State:\n");
    RUN_TEST(pwm_init_starts_at_zero);
    RUN_TEST(pwm_step_increments);
    RUN_TEST(pwm_step_reverses_at_max);
    RUN_TEST(pwm_step_reverses_at_min);
    RUN_TEST(pwm_full_cycle_up);
    RUN_TEST(pwm_full_cycle_down);
    
    printf("\nDuty Cycle:\n");
    RUN_TEST(pwm_duty_cycle_uses_gamma);
    RUN_TEST(pwm_duty_cycle_at_zero);
    RUN_TEST(pwm_duty_cycle_at_max);
    
    printf("\nHardware Interaction:\n");
    RUN_TEST(pwm_cycle_correct_total_timing);
    RUN_TEST(pwm_cycle_zero_brightness_led_off);
    RUN_TEST(pwm_cycle_max_brightness_led_on);
    RUN_TEST(pwm_cycle_mid_brightness_toggles);
    
    printf("\nIntegration:\n");
    RUN_TEST(full_fade_cycle_timing_consistent);
    
    printf("\n=================================================\n");
    printf("  Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("=================================================\n\n");
    
    return (tests_passed == tests_run) ? 0 : 1;
}
