//
// Created by oscar on 14/05/19.
//
#include "tone.h"
#include "pinout.h"
#include "melody.h"
#include "macros.h"

ISR(TIMER3_COMPA_vect) {
        tbi(PORT_BUZ,BUZ);
}

void tone(unsigned int freq) {
    if (freq < 10) {
        noTone();
        return;
    }

    unsigned int counter = 1e6/freq/2;

    // Enable compare A on timer 3
    sbi(TIMSK3, OCIE3A);

    OCR3A = counter;
}

void toneSetup() {
    sbi(DDR_BUZ,BUZ);

    cli();

    // CTC Mode Timer 3
    // Clear TCNT3 on OCR3A match
    cbi(TCCR3A, WGM30);
    cbi(TCCR3A, WGM31);
    sbi(TCCR3B, WGM32);
    cbi(TCCR3B, WGM33);

    // clk/8 prescaler. 1us each TCNT3 increment
    cbi(TCCR3B, CS30);
    sbi(TCCR3B, CS31);
    cbi(TCCR3B, CS32);

    OCR3A = 0xFFFF;
    TCNT3 = 0;

    sei();
}

void noTone() {
    // Disable compare A on timer 3
    cbi(TIMSK3, OCIE3A);
    cbi(PORT_BUZ,BUZ);
}

