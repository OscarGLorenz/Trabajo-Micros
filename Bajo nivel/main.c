#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#include "../charpy/time.h"
#include "../charpy/pinout.h"

using namespace charpy::time;


volatile int pos = 0;
volatile bool dir = 0;
volatile long int now = 0;
volatile long int past = 0;
volatile float filtr = 0;
volatile long int dif = 0;
volatile long int lastIn = 0;

volatile int difference = 0;
volatile int lastPos = 0;

volatile bool pulsDir = 0;
#define Kfiltr 0.95


volatile bool changedDir = false;
ISR(INT3_vect) {
        tbi(OUTRUT,L1);

        if (dir) pos++;
        else pos--;
        now = millis();
        dif = now - past;
        filtr = (1-Kfiltr)*dif + Kfiltr *filtr;

        if ( dif > 2*filtr && !changedDir) {
            //if (now - lastIn > 300) {
                changedDir = true;
                dir = !dir;
                tbi(OUTRUT,M2_di); // Toggle M2_dir
             //   lastIn = now;
            //}
        } else if ( dif < 2*filtr && changedDir) {
            changedDir = false;
        }

        past = now;
}


volatile bool flag = 0;
volatile long int nowInter = 0;
ISR(PCINT0_vect) {
    if (rbi(PINRUT,SO5)) {
            flag = 1;
            nowInter = millis();
    }
}

void setup() {
    OUTRUT_DDR = 0xFF;

    cli();

    // CHANGE triggers INT3
    sbi(EICRA, ISC30);
    cbi(EICRA, ISC31);

    // Enable INT3
    sbi(EIMSK, SO4);

    // Enable PCINT0
    sbi(PCMSK0, SO5);

    sei();


    sbi(OUTRUT,M2_en);

    while(pos < 8) {
        if (sin(5.5*(millis()/1000.0)) >0)
            sbi(OUTRUT,M2_di);
        else
            cbi(OUTRUT,M2_di);
    }

    sbi(OUTRUT, L2);

}


int duty = 0;
//long int pocoPoco = 0;


void loop() {
    if ( millis() - nowInter > 1 && flag) {
        if (rbi(PINRUT,SO5))  {
            pos = 0;
        }
        flag = 0;
    }

//    if (millis() - pocoPoco > 5) {
//        pocoPoco = millis();
//        difference = pos - lastPos;
//        lastPos = pos;
//    }

}



int main() {
    init();
    setup();

    for(;;)
        loop();

    return 0;
}