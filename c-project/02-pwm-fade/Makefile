# PWM Fade project - with host-based testing

TARGET = main

# AVR sources (for device)
AVR_SRCS = main.c pwm.c hal_avr.c

# Test sources (for host)
TEST_SRCS = test/test_pwm.c pwm.c hal_mock.c

# Host compiler for tests
HOST_CC = gcc
HOST_CFLAGS = -Wall -Wextra -std=c99 -DTEST_BUILD -g

# Default: build for AVR
all: $(TARGET).hex

# AVR build
$(TARGET).o: main.c pwm.h hal.h
	$(CC) $(CFLAGS) -c -o $@ main.c

pwm.o: pwm.c pwm.h hal.h
	$(CC) $(CFLAGS) -c -o $@ pwm.c

hal_avr.o: hal_avr.c hal.h
	$(CC) $(CFLAGS) -c -o $@ hal_avr.c

$(TARGET).elf: $(TARGET).o pwm.o hal_avr.o
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --mcu=$(MCU) -C $<

# Host-based tests
test: test_runner
	./test_runner

test_runner: test/test_pwm.c pwm.c hal_mock.c pwm.h hal.h
	$(HOST_CC) $(HOST_CFLAGS) -o $@ test/test_pwm.c pwm.c hal_mock.c

clean:
	rm -f *.o *.elf *.hex test_runner

include ../common.mk

.PHONY: all test clean
