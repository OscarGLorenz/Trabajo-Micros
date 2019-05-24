/*************************************************************
*
* FICHERO: serial.c
*
* DESCRIPCIÓN :
* Conjunto de funciones necesarias para la inicialización del Serial
* y para su uso posterior, tanto como entrada como salida.
*
*
* AUTORES:
*
* 	Miguel Fernández Cortizas
*   Óscar García Lorenz
*   Mario Musicó Cortés
*
*
*************************************************************/

#include "serial.h"

#define F_CPU 8000000
#include "macros.h"
#include <stdlib.h>
#include <avr/io.h>

// Inicializacion del serial en funcion del los baudios.
void serialBegin(unsigned long baud) {
  // Fast mode
  sbi(UCSR2A,U2X2);

  // Enable transmit & receive
  sbi(UCSR2B,TXEN2);
  sbi(UCSR2B,RXEN2);

  // 8 bit, no parity, asyncronous and 1 stop bit
  sbi(UCSR2C,UCSZ20);
  sbi(UCSR2C,UCSZ21);

  // baud configuration
  const uint16_t baud_setting = ((F_CPU/(baud*8UL))-1);
  UBRR2H = (baud_setting >> 8);
  UBRR2L = baud_setting;
}

// Envío de un carácter a traves de Serial
void serialWrite(char data) {
  while (!rbi(TX_CHECK_REG,TX_CHECK_BIT)); // wait for the charater to be fully received
  SERIAL_TX = data;
}

// Envío de una cadena de caracteres
void serialPrint(const char * str) {
  for (int i = 0; str[i] != 0; i++)
    serialWrite(str[i]);
}

// Envío de una cadena de caracteres + un salto de linea
void serialPrintLn(const char * str) {
    serialPrint(str);
    serialWrite('\n');
}

// Envio de un enter
void serialPrintInt(int i) {
    char buf[10];
    itoa(i,buf,10);
    serialPrint(buf);
}

// Envío de un Uint
void serialPrintUInt(unsigned int i) {
    char buf[10];
    utoa(i,buf,10);
    serialPrint(buf);
}

// Envio de un long
void serialPrintLong(long i) {
    char buf[20];
    ltoa(i,buf,10);
    serialPrint(buf);
}

// Envío de un ulong
void serialPrintULong(unsigned long i) {
    char buf[20];
    ultoa(i,buf,10);
    serialPrint(buf);
}

//Envio de un float
void serialPrintFloat(float f) {
    char buf[10];
    dtostrf(f,8,4,buf);
    serialPrint(buf);
}

// Recepción de un caracter a través del puerto Serie
unsigned char serialReadChar(){
  if(rbi(RX_CHECK_REG,RX_CHECK_BIT)) return(SERIAL_RX); //wait for the charater to be fully received
  else return 0;        //return the received charater
}
// Recepcion de una cadena de caracteres con un timeout.
void serialReadString(char* command, int timeout){
  unsigned char x;
  uint8_t i = 0, reading = 1;
  do{ //receive the characters until ENT  ER is pressed (ASCII for ENTER = 13)
	   while(!(x = serialReadChar()) && timeout--);
	   if ((x) && (x != '\n')) command[i++] = x; //and store the received characters into the array string[] one-by-one
	   else reading = 0;
  } while (reading);
  command[i] = '\0'; //insert NULL to terminate the string
}
