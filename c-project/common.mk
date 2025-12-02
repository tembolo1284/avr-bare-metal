# Common settings for Arduino Uno (ATmega328P)

MCU      = atmega328p
F_CPU    = 16000000UL
BAUD     = 115200
PORT    ?= /dev/ttyACM0

CC       = avr-gcc
OBJCOPY  = avr-objcopy
AVRDUDE  = avrdude
SIZE     = avr-size

CFLAGS   = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -std=c99

# Build rules (TARGET must be defined by including Makefile)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.elf: %.o
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --mcu=$(MCU) -C $<

flash: $(TARGET).hex
	sudo $(AVRDUDE) -F -V -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<

clean:
	rm -f *.o *.elf *.hex

.PHONY: flash clean
