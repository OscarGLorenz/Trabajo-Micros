#include "serial.h"

#define F_CPU 8000000
#include "macros.h"
#include <stdlib.h>
#include <avr/io.h>

void serialBegin(unsigned long baud) {  
  // Fast mode
  sbi(UCSR2A,U2X2);

  // Enable transmit & receive
  sbi(UCSR2B,TXEN2);
  sbi(UCSR2B,RXEN2);
   
  // 8 bit, no parity, asyncronous and 1 stop bit
  sbi(UCSR2C,UCSZ20); 
  sbi(UCSR2C,UCSZ21); 
  
  const uint16_t baud_setting = ((F_CPU/(baud*8UL))-1);
  UBRR2H = (baud_setting >> 8);
  UBRR2L = baud_setting;
}

void serialWrite(char data) {
  while (!rbi(UCSR2A,UDRE2)); // wait for the charater to be fully received
  UDR2 = data;
}

void serialPrint(const char * str) {
  for (int i = 0; str[i] != 0; i++) 
    serialWrite(str[i]);
}

void serialPrintLn(const char * str) {
    serialPrint(str);
    serialWrite('\n');
}

void serialPrintInt(int i) {
    char buf[10];
    itoa(i,buf,10);
    serialPrint(buf);
}

void serialPrintFloat(float f) {
    char buf[10];
    dtostrf(f,8,4,buf);
    serialPrint(buf);
}

unsigned char serialReadChar(){
	while(!rbi(UCSR2A,RXC2))); 	//wait for the charater to be fully received
	return(UDR2); 				//return the received charater
}

unsigned char * serialReadStringUntil(char endCharacter){
	unsigned char string[20], x;
	uint8_t i = 0;
	while((x = serialReadChar() != endCharacter){ //receive the characters until ENTER is pressed (ASCII for ENTER = 13)
		string[i++] = x; //and store the received characters into the array string[] one-by-one
	}
	string[i] = '\0'; //insert NULL to terminate the string
	return(string); //return the received string
}

unsigned char * serialReadString(){
	return serialReadStringUntil('\n');
}
	
