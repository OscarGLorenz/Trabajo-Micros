#ifdef SERIAL_H_
#define SERIAL_H_

#define F_CPU 16000000
void serialBegin(unsigned long baud) {  
  // Fast mode
  sbi(UCSR2A,U2X0);

  // Enable rx and tx. Enable receive interrupt
  sbi(UCSR2B,RXEN2);
  sbi(UCSR2B,TXEN2); 
   
  // 8 bit, no parity, asyncronous and 1 stop bit
  sbi(UCSR2C,UCSZ20); 
  sbi(UCSR2C,UCSZ21); 
  
  const uint16_t baud_setting = ((F_CPU/(baud*8UL))-1);
  UBRR2H = (baud_setting >> 8);
  UBRR2L = baud_setting;
}

void serialWrite(char data) {
  while (!rbi(UCSR2A,UDRE2)); // Wait until register is empty
  UDR0 = data;

}

void serialPrint(const char * str) {
  for (int i = 0; str[i] != 0; i++) 
    serialWrite(str[i]);
}

#endif
