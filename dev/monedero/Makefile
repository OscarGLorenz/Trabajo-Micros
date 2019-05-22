CC = avr-gcc
CFLAGS = -Wall -g -Os
MCU = atmega640
AVRDUDEMCU = m640
PROGRAMMER = dragon_jtag
LIB = -I ../../src/common
main:
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o main.o main.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o monedero.o monedero.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o time.o ../../src/common/time.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o serial.o ../../src/common/serial.c

	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) serial.o main.o monedero.o time.o -o main.bin
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"main.hex":a

