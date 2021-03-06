#ifndef SERIAL_H_
#define SERIAL_H_

#define F_CPU 8000000
#include <stdlib.h>

void serialBegin(unsigned long baud);

void serialWrite(char data);
void serialPrint(const char * str);
void serialPrintLn(const char * str);
void serialPrintInt(int i);
void serialPrintFloat(float f);

unsigned char serialReadChar();
unsigned char * serialReadStringUntil(char endCharacter);
unsigned char * serialReadString();

#endif
