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

enum mode_t {CUELGA,ROTA,GIRA,FRENA,WAIT};
enum mode_t mode = CUELGA;

float times[5];
float bouncing_time=500;
ISR(INT3_vect){

        if (dir) pos++;
        else pos--;
        now = millis();
        dif = now - past;

        for (int j=0;j<4;j++){
          times[j]=times[j+1];
        }
        times[4] = dif;

      int aux = (((times[0]+times[1]) * 2.5) - (times[2]+times[3]+times[4]));

      if (now - lastIn > bouncing_time){
        if (aux < 0){

          if (pos > 40  || pos < -40 ){
            bouncing_time= 3 *(times[2]+times[3]+times[4]);
            if (bouncing_time < 500) bouncing_time = 500;
          }else bouncing_time = 500;

          dir = !dir;
          tbi(OUTRUT,M2_di); // Toggle M2_dir
          lastIn = now;
        }
      }

      past = now;

}

unsigned long start_flag=0;
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
    start_flag= millis();

}


int duty = 0;
long int pocoPoco = 0;
long int auxTime = 0;

void loop() {
    switch(mode) {
        case CUELGA:

            if(pos < 10) {
                if (millis() - start_flag < 1000){
                  cbi(OUTRUT,M2_di);
                }
                else if (millis()%1200 > 1200/2)
                    sbi(OUTRUT,M2_di);
                else
                    cbi(OUTRUT,M2_di);
            } else {
                mode = ROTA;
                lastIn = now;
                sbi(OUTRUT, L2);
            }
            break;

        case ROTA:

            if (millis() - lastIn > 6000) {
              mode = FRENA;
              dir = !dir;
              tbi(OUTRUT,M2_di); // Toggle M2_dir
              cbi(OUTRUT, L2);
              auxTime = millis();
            }
            break;
        case FRENA:
            if (millis() - auxTime > 3000) {
              mode = WAIT;
              cbi(OUTRUT,M2_en);
              cbi(OUTRUT,M2_di);
              sbi(OUTRUT, L4);

            }
            break;
        case WAIT:
            break;
    }
    // PCINT MOAR CODE
    if ( millis() - nowInter > 1 && flag) {
        if (rbi(PINB,PB0)) {
            pos = +6;
        }
        flag = 0;
    }
    if (!rbi(PINK,SW1)) {
      mode = FRENA;
      dir = !dir;
      tbi(OUTRUT,M2_di); // Toggle M2_dir
      cbi(OUTRUT, L2);
      auxTime = millis();
    }

    if (millis() - pocoPoco > 5) {
        pocoPoco = millis();
        //serialPrintFloat((float) pos);
        serialPrintFloat(dif);
        serialWrite(' ');
        serialPrintFloat(pos);
        serialWrite(' ');
        serialPrintFloat(bouncing_time);
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
