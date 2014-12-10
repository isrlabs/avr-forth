#############
# TOOLCHAIN #
#############

CC = avr-gcc
LD = avr-ld
STRIP = avr-strip
OBJCOPY = avr-objcopy
SOURCES = $(wildcard *.c)
SIZE = avr-size


##############
# PARAMETERS #
##############

TARGET = avrforth
PROGRAMMER = avrisp2
MCU = atmega328
PART = m328p
F_CPU = 16000000
PORT = usb
BAUD = 9600
BINFORMAT = ihex
PGMPROTO = avrisp2
CFLAGS = -Wall -O2 -DF_CPU=$(F_CPU) -mmcu=$(MCU) -I. -DBAUD=$(BAUD)
AVRDUDE = avrdude -v -p $(PART) -c $(PROGRAMMER) -P $(PORT)

all: $(TARGET).hex

info:
	@echo "sources: $(SOURCES)"

.elf.hex:

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O $(BINFORMAT) -R .eeprom $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).hex

$(TARGET).elf: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET).elf $(SOURCES)
	$(STRIP) $(TARGET).elf

upload: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$(TARGET).hex

clean:
	rm -f *.hex *.elf *.eeprom

.PHONY: clean upload
