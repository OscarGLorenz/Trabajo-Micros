#include "serialc.h"

#define F_CPU 8000000
#include <stdlib.h>
#include <avr/io.h>

void serialBegin(unsigned long baud) {  
  // Fast mode
  sbi(UCSR0A,U2X0);

  // Enable transmit & receive
  sbi(UCSR0B,TXEN0);
  sbi(UCSR0B,RXEN0);
   
  // 8 bit, no parity, asyncronous and 1 stop bit
  sbi(UCSR0C,UCSZ00); 
  sbi(UCSR0C,UCSZ01); 
  
  const uint16_t baud_setting = ((F_CPU/(baud*8UL))-1);
  UBRR0H = (baud_setting >> 8);
  UBRR0L = baud_setting;
}

void serialWrite(char data) {
  while (!rbi(UCSR0A,UDRE0)); // wait for the charater to be fully received
  UDR0 = data;
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
	while(!rbi(UCSR0A,RXC0)); 	//wait for the charater to be fully received
	return(UDR0); 				//return the received charater
}

unsigned char * serialReadStringUntil(char endCharacter){
	unsigned char string[20], x;
	uint8_t i = 0;
	while(x = serialReadChar() != endCharacter){ //receive the characters until ENTER is pressed (ASCII for ENTER = 13)
		string[i++] = x; //and store the received characters into the array string[] one-by-one
	}
	string[i] = '\0'; //insert NULL to terminate the string
	return(string); //return the received string
}

unsigned char * serialReadString(){
	return serialReadStringUntil('\n');
}
	
