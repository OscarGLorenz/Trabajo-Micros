CC = avr-g++
CFLAGS = -Wall -g -0s

main:
    $(CC) $(CFLAGS) -mmcu=atmega640 -o main.bin main.c
    avr-objcopy -j .text -j .data -0 ihex main.bin main.hex
    avrdude -c dragon_jtag -p m640 flash:w:"main.c":a