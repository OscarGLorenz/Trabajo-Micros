#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include "../charpy/macros.h"
#include "../charpy/time.h"
#include "../charpy/pinout.h"
#include "../charpy/serial.h"

// Variables auxiliares para el encoder
volatile int pos = 0;
volatile bool dir = 0;
volatile long int now = 0;
volatile long int past = 0;
volatile long int dif = 0;
volatile long int lastIn = 0;

enum mode_t {ESPERA,CUELGA,ROTA,GIRA,FRENA,EMERGENCIA,CATASTROFE,LOBOTOMIA,CARGA};
enum mode_t mode = ESPERA;

bool L4on = false;
unsigned int  L4tiempo = 0 ;

float times[5];
float bouncing_time=500;
ISR(SO4_vect){

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
ISR(SO5_vect) {

        if (rbi(PIN_S05,P_S05)) {
            flag = 1;
            nowInter = millis();
        }
}

void setup() {

    DDR_OUTRUT = 0xFF;

    cli();

    // CHANGE triggers INT3
    sbi(CTRL_INT, SO4_C0);
    cbi(CTRL_INT, SO4_C1);

    // Enable INT3
    sbi(INT_MASK, SO4);

    // Enable PCINT0
    sbi(S05_CTRL, SO5_ENABLE);
    sbi(SO5_MASK, SO5);

    sei();




    serialBegin(9600);
    start_flag= millis();

}


int duty = 0;
long int pocoPoco = 0;
long int auxTime = 0;

void loop() {
    switch(mode) {
        case ESPERA:
          if (!rbi(PINRUT,SW3)) {
            mode=CARGA;
            auxTime=millis();
            sbi(OUTRUT,L3);
          }
        break;

          case CARGA :
            if (millis() - auxTime > 1000 ){
              mode = CUELGA;
              sbi(OUTRUT,M2_en);
              cbi(OUTRUT,L3);
            }
          break;

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
            }
            break;

        case ROTA:
            if (millis() - lastIn > 2000) {
              mode = GIRA;
            }
            break;
        case GIRA:
          if (millis() - lastIn > 10000) {
            mode = FRENA;
            dir = !dir;
            tbi(OUTRUT,M2_di); // Toggle M2_dir
            auxTime = millis();
          }
        break;

        case FRENA:
            if (millis() - auxTime > 12000) {
              mode = ESPERA;
              cbi(OUTRUT,M2_en);
              cbi(OUTRUT,M2_di);

            }
            break;

            case CATASTROFE:
            if (millis() - auxTime > 5000) {
              mode = EMERGENCIA;
              auxTime = millis();
            }

            break;
            case EMERGENCIA:
            if (millis() - auxTime > 5000) {
              mode = LOBOTOMIA;
            }

            break;
            case LOBOTOMIA:
              cbi(OUTRUT,M2_en);
              cbi(OUTRUT,M2_di);
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
      if (mode == ROTA) {
        mode = EMERGENCIA;
        dir = !dir;
        tbi(OUTRUT,M2_di); // Toggle M2_dir
        auxTime = millis();
      } else if (mode==CUELGA || mode==ESPERA) {
        mode = LOBOTOMIA;
      }else if(mode == FRENA) mode = EMERGENCIA;
       else if (mode == GIRA) {
         mode = CATASTROFE;
         dir = !dir;
         tbi(OUTRUT,M2_di); // Toggle M2_dir
         auxTime = millis();

    }
  }
  // luces

  if (mode == CUELGA ||mode == ROTA ||mode == GIRA ||mode == FRENA){
    if (L4on){
      if (millis()-L4tiempo > 500){
        L4on = false;
        cbi(OUTRUT,L4);
        L4tiempo = millis();
      }
    }else{
      if (millis()-L4tiempo > 10000){
        L4on = true;
        sbi(OUTRUT,L4);
        L4tiempo = millis();

      }
    }
  }
  else if (mode == LOBOTOMIA ||mode == CATASTROFE||mode == EMERGENCIA ){
    if (L4on){
      if (millis()-L4tiempo > 200){
        L4on = false;
        cbi(OUTRUT,L4);
        L4tiempo = millis();

      }
    }else{
      if (millis()-L4tiempo > 1000){
        L4on = true;
        sbi(OUTRUT,L4);
        L4tiempo = millis();

      }
    }
  } else {
    cbi(OUTRUT,L4);
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
        serialWrite(' ');
        serialPrintFloat(((int) mode)*100);
        serialWrite('\n');
    }

}



int main() {
    initTime();
    setup();

    for(;;)
        loop();

    return 0;
}
