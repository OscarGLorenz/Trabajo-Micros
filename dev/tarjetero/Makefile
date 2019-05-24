CC = avr-g++
CFLAGS = -Wall -g -O0
MCU = atmega640
AVRDUDEMCU = m640
PROGRAMMER = dragon_jtag
LIB = -I ../../src/common
main:
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o time.o ../../src/common/time.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o serial.o ../../src/common/serial.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) time.o serial.o -o main.bin main.c
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"main.hex":a

tone:
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o time.o ../../src/common/time.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) -c -o tone.o tone.c
	$(CC) $(LIB) $(CFLAGS) -mmcu=$(MCU) tone.o time.o -o main.o
	avr-objcopy -j .text -j .data -O ihex main.o main.hex
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"main.hex":a