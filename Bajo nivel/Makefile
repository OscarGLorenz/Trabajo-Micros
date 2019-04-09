CC = avr-g++
CFLAGS = -Wall -g -Os
MCU = atmega640
AVRDUDEMCU = m640
PROGRAMMER = dragon_jtag
main:
	$(CC) $(CFLAGS) -mmcu=$(MCU) -o main.bin main.c
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c $(PROGRAMMER) -p $(AVRDUDEMCU) -U flash:w:"main.hex":a
