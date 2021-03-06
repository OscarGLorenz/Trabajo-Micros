/*************************************************************
*
* FICHERO: time.c
*
* DESCRIPCIÓN :
* Libreria que permite medir tiempos en microsegundos y en milisegundos empleando el TIMER1.
*
*
* AUTORES:
*
*   Óscar García Lorenz
*   Miguel Fernández Cortizas
*
*************************************************************/




#include "time.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pinout.h"
#include "macros.h"

static volatile unsigned long millis_cnt = 0;

// Tiempo en milisegundos
unsigned long millis() {
  return millis_cnt;
}

// Tiempo en microsegundos
unsigned long micros() {
  return millis_cnt * 1000 + TCNT1;
}

// Esta interrupción salta cada ms
ISR(TIMER1_COMPA_vect) {
  millis_cnt++;
}

// Rutina de configuración
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
