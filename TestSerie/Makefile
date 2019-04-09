CC = avr-g++
CFLAGS = -Wall -g -Os

main:
	$(CC) $(CFLAGS) -mmcu=atmega640 -o main.bin main.c
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c dragon_jtag -p m640 -U flash:w:"main.hex":a
