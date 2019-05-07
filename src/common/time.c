#include "time.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pinout.h"
#include "macros.h"

volatile unsigned long millis_cnt = 0;

unsigned long millis() {
  return millis_cnt;
}

unsigned long micros() {
  return millis_cnt * 1000 + TCNT1;
}


ISR(TIMER1_COMPA_vect) {
  millis_cnt++;
}

void initTime() {
  cli();

  // CTC MODE ON TIMER 1
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

  OCR1A = 1000; // 1 ms

  TCNT1 = 0;

  sei();
}



