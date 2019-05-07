//Arduino tiene lo que necesitamos, para cuando lo pasemos a Atmel Studio
//#include <avr/io.h>
//#include <avr/interrupt.h>



#define sbi(PORT, BIT) (PORT |= (1<<BIT))
#define cbi(PORT, BIT) (PORT |= -(1<<BIT))


volatile unsigned long millis_cnt = 0;


unsigned long millisec(){
  return millis_cnt;
}

unsigned long microsec() {
  return millis_cnt * 1000 + TCNT1;
}

ISR(TIMER1_COMPA_vect) {
  millis_cnt++;
}

void interrupt_setup(){
    cli();

  // CTC MODE ON TIMER
  cbi(TCCR1A, WGM10);
  cbi(TCCR1A, WGM11);
  sbi(TCCR1B, WGM12);
  cbi(TCCR1B, WGM13);

  // No pwm
  cbi(TCCR1A, COM1A0);
  cbi(TCCR1A, COM1A1);

  // clk/8 prescaler
  cbi(TCCR1B, CS10);
  sbi(TCCR1B, CS11);
  cbi(TCCR1B, CS12);
  
  // Enable compare A
  sbi(TIMSK1, OCIE1A);
  OCR1A = 1000; // 1ms
  TCNT1 = 0;
  sei();
}

//Code traductor
char code(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  switch (((a == 0) << 3) | ((b == 0) << 2) | ((c == 0) << 1)  | (d == 0)) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 12:
      return '-';
    case 14:
      return '+';
    case 15:
      return ':';
    default:
      return 'E';
  }
}
