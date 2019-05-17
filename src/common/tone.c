//
// Created by oscar on 14/05/19.
//
#include "tone.h"
#include "pinout.h"
#include "melody.h"
#include "time.h"
#include "macros.h"
#include <string.h>
#include <stdlib.h>

volatile long times = 0;
volatile unsigned int note = 0;
Song * song;

void setupMelody(Song *  s, const unsigned int * note, const unsigned int * del,  const unsigned int n) {
    s->notes =(unsigned int *) malloc(2*n);
    memcpy(s->notes,note,n);
    s->delays = (unsigned int *) malloc(2*n);
    memcpy(s->delays,del,n);
    s->n = n;
}

ISR(TIMER3_COMPA_vect) {
   tbi(OUTRUT,M2_en);
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
    sbi(DDR_OUTRUT,M2_en);

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


    setupMelody(&tetris,tetrisNotas,tetrisDelays,tetrisN);
}



void noTone() {
    // Disable compare A on timer 3
    cbi(TIMSK3, OCIE3A);
    cbi(PORT_BUZ,BUZ);
}


unsigned long ultimaNota = 0;
unsigned int nota = 0;
void sonidito() {
    if ((millis() - ultimaNota > tetrisDelays[nota-1]*2)) {
		tone(tetrisNotas[nota]);
		ultimaNota = millis();
		nota++;
		if (nota > tetris.n) noTone();
	}
}

