#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include "../charpy/macros.h"
#include "../charpy/time.h"
#include "../charpy/pinout.h"
#include "../charpy/serial.h"


volatile int pos = 0;
volatile bool dir = 0;
volatile long int now = 0;
volatile long int past = 0;
volatile float filtr = 0;
volatile long int dif = 0;
volatile long int lastIn = 0;

volatile int difference = 0;
volatile int lastPos = 0;
volatile int dif_1 = 0;
volatile int dif_2 = 0;

volatile bool pulsDir = 0;
#define Kfiltr 0.95


volatile bool changedDir = false;

enum mode_t {START,ACCEL,BRAKE};
enum mode_t mode = START;

ISR(INT3_vect) {

        if (dir) pos++;
        else pos--;
        now = millis();
        dif = now - past;
        filtr = (1-Kfiltr)*dif + Kfiltr *filtr;

        if ( dif > 1.8*filtr && !changedDir) {
            if (now - lastIn >  500) {
                changedDir = true;
                dir = !dir;
                tbi(OUTRUT,M2_di); // Toggle M2_dir
                lastIn = now;
            }
        } else if ( dif < 2*filtr && changedDir) {
            changedDir = false;
        }

        past = now;
}


volatile bool flag = 0;
volatile long int nowInter = 0;
ISR(PCINT0_vect) {

        if (rbi(PINB,PB0)) {
            tbi(OUTRUT,L1);

            flag = 1;
            nowInter = millis();
        }
}

void setup() {
    while (rbi(PINK,SW1));

    DDRL = 0xFF;

    cli();

    // CHANGE triggers INT3
    sbi(EICRA, ISC30);
    cbi(EICRA, ISC31);

    // Enable INT3
    sbi(EIMSK, SO4);

    // Enable PCINT0
    sbi(PCICR, PCIE0);
    sbi(PCMSK0, PCINT0);

    sei();


    sbi(OUTRUT,M2_en);


    serialBegin(9600);



}


int duty = 0;
long int pocoPoco = 0;


void loop() {
    switch(mode) {
        case START:
            if(pos < 10) {
                if (millis()%1200 > 1200/2)
                    sbi(OUTRUT,M2_di);
                else
                    cbi(OUTRUT,M2_di);
            } else {
                mode = ACCEL;
                sbi(OUTRUT, L2);
            }
            break;

        case ACCEL:
            if ( millis() - nowInter > 1 && flag) {
                if (rbi(PINB,PB0)) {
                    pos = 0;
                }
                flag = 0;
            }
            break;
    }

    if (millis() - pocoPoco > 5) {
        pocoPoco = millis();
        //serialPrintFloat((float) pos);
        serialPrintFloat(1.8*filtr);

        serialWrite(' ');
        serialPrintFloat(dif);
        serialWrite(' ');

        serialPrintFloat((dir) ? 50.0 : -50.0);
        serialWrite('\n');
        lastPos = pos;
    }

}



int main() {
    initTime();
    setup();

    for(;;)
        loop();

    return 0;
}
