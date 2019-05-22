CC := avr-g++
CFLAGS = -Wall -g -Os
MCU = atmega640
AVRDUDEMCU = m640
PROGRAMMER = dragon_jtag

BUILDDIR := build
SRCDIR := src
TARGET := bin/main.o
HEX := bin/main.hex


SRCEXT := c
SOURCES := $(shell find src -type f -name *.c)
ASM := $(shell find src -type f -name *.s)
ASM_OBJECTS := $(patsubst src/%,build/%,$(ASM:.s=.o))
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
INC := -I src/common

$(TARGET): $(OBJECTS) $(ASM_OBJECTS)
	$(CC) -mmcu=$(MCU) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.s
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o $@ $<

upload:
	avr-objcopy -j .text -j .data -O ihex $(TARGET) $(HEX)
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"$(HEX)":a

init:
	mkdir -p build/common
	mkdir -p build/atraccion
	mkdir -p build/tarjetero
	mkdir -p build/monedero
	mkdir -p bin

clean:
	rm -r build/
	make init

.PHONY: clean
