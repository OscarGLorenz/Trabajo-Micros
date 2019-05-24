/***********************************************************************************
*
* FICHERO: Makefile
*
* DESCRIPCIÓN :
*  make:
* 	Este archivo ejecuta la compilación del proyecto. Primero compila cada
*   archivo .c por separado o .s y al final une todos los archivos objetos.
*  make upload:
*   Sube el program al micro utilizando el avr dragon
*  make clean:
*   Limpia los archivos objeto y el programa compilado
*  make init:
*   Crea las carpetas necesarias para llevar a cabo la compilación
*
* AUTORES:
*
*   Óscar García Lorenz
*
***********************************************************************************/

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
ASM_OBJECTS := build/monedero/monedero_asm.o
OBJECTS := build/main.o build/common/time.o build/common/serial.o build/atraccion/atraccion.o build/monedero/monedero.o build/monedero/monedero.o build/tarjetero/tarjetero.o
INC := -I src/common

$(TARGET): $(OBJECTS) $(ASM_OBJECTS)
	$(CC) -mmcu=$(MCU) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -mmcu=$(MCU) $(INC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.S
	$(CC) -mmcu=$(MCU) $(CFLAGS) -c -o $@ $<

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
