
/*************************************************************
*
* FICHERO: serial.h
*
* DESCRIPCIÓN :
* 	Contiene las macros necesarias para la inicialización del Serial y los prototipos de las funciones Serial.
*
*
* AUTORES:
*
* 	Miguel Fernández Cortizas
*
*   Óscar García Lorenz
*
*
*************************************************************/


#ifndef SERIAL_H_
#define SERIAL_H_

#define F_CPU 8000000

#define SERIAL_TX UDR2
#define TX_CHECK_REG UCSR2A
#define TX_CHECK_BIT UDRE2

#define SERIAL_RX UDR2
#define RX_CHECK_REG UCSR2A
#define RX_CHECK_BIT RXC2

#include "macros.h"
#include <stdlib.h>

void serialBegin(unsigned long baud);

void serialWrite(char data);
void serialPrint(const char * str);
void serialPrintLn(const char * str);
void serialPrintInt(int i);
void serialPrintUInt(unsigned int i);
void serialPrintLong(long i);
void serialPrintULong(unsigned long i);
void serialPrintFloat(float f);

unsigned char serialReadChar();
void serialReadString(char * command, int timeout);

#endif
