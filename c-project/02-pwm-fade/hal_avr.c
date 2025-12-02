/*
 * hal_avr.c - Real AVR hardware implementation
 */

#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>

void hal_init(void) {
    // Set PB5 as output
    DDRB |= (1 << PB5);
}

void hal_set_led(bool on) {
    if (on) {
        PORTB |= (1 << PB5);
    } else {
        PORTB &= ~(1 << PB5);
    }
}

void hal_delay_us(uint16_t us) {
    // _delay_us requires a compile-time constant, so we loop
    while (us--) {
        _delay_us(1);
    }
}
