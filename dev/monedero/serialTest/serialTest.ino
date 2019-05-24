//#include <avr/io.h>
//#include "monedero.h"
#include "macros.h"
#include <string.h>

char command[20]="Hola";

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
  if(rbi(UCSR0A,RXC0)){
   // cbi(UCSR0A,RXC0);
    return(UDR0); //wait for the charater to be fully received
  }
  else return 0;        //return the received charater
}

void serialReadStringUntil(char* command, char endCharacter){
  unsigned char string[20], x;
  uint8_t i = 0, reading = 1;
  do{ //receive the characters until ENTER is pressed (ASCII for ENTER = 13)
   x = serialReadChar();
   if ((x) && (x != endCharacter)){
    command[i++] = x; //and store the received characters into the array string[] one-by-one
   }
   else reading = 0;
  } while (reading);
  command[i] = '\0'; //insert NULL to terminate the string
}

void serialReadString(char* command){
  serialReadStringUntil(command,'\n');
}
  
void serialWatchdog(char* command){
  static uint8_t delay = 0;
  if(delay < 255) {
    delay++;
  }
  else {
    delay = 0;
    if (rbi(UCSR0A,RXC0)){
    serialReadString(command);
    serialPrint(command);
    if(!strcmp(command,"cal"))serialPrintLn(": CALIBRATE MODE");
    else if (!strcmp(command,"run"))serialPrintLn(": RUN MODE");
    else if (!strcmp(command,"2"))serialPrintLn(": configuring 2c coin");
    else if (!strcmp(command,"5"))serialPrintLn(": configuring 5c coin");
    else if (!strcmp(command,"10"))serialPrintLn(": configuring 10c coin");
    else if (!strcmp(command,"20"))serialPrintLn(": configuring 20c coin");
    else if (!strcmp(command,"50"))serialPrintLn(": configuring 50c coin");
    else if (!strcmp(command,"100"))serialPrintLn(": configuring 100c coin");
    else if (!strcmp(command,"100"))serialPrintLn(": configuring 200c coin");
    else serialPrintLn(": error, command not available");
    }
  }
}

void setup(){
  serialBegin(57600);
}

void loop(){
  serialWatchdog(command);
  delay(1);
 // serialPrint("Comando: ");
 // serialPrintLn(command);
}
