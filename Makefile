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
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
INC := -I src/common

main:
	@mkdir -p bin
	@mkdir -p build/monedero
	@mkdir -p build/atraccion
	@mkdir -p build/tarjetero
	@mkdir -p build/common
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/monedero.o src/monedero/monedero.c 
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/tarjetero.o src/tarjetero/tarjetero.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/atraccion.o src/atraccion/atraccion.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/main.o src/main.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/common/time.o src/common/time.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o build/common/serial.o src/common/serial.c
	$(CC) -mmcu=$(MCU) $(INC) build/common/serial.o build/atraccion.o build/tarjetero.o build/monedero.o build/common/time.o build/main.o -o bin/main.o
	

upload:
	avr-objcopy -j .text -j .data -O ihex $(TARGET) $(HEX)
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"$(HEX)":a

clean:
	rm -r build/

.PHONY: clean
