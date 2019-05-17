/*
   Tarjetero.c

   Created: 08/05/2019 11:57:37
   Author:
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "time.h"
#include "pinout.h"
#include "serial.h"
#include "macros.h"
#define HIGH 1
#define LOW 0

#define NUM_TOTAL_RAYITAS 26    //NÚMERO DE RAYAS QUE NOS ENCONTRAMOS POR TARJETA
#define LRG 0           //DEFINIMOS RAYAS LARGAS CON CÓDIGO 0
#define CRT 1           //DEFINIMOS RAYAS CORTAS CON CÓDIGO 1

int bandera1;                             //FLANCO DE INICIO DE RAYA DETECTADO
int bandera2;                           //FLANCO DE FIN DE RAYA DETECTADO
int num_rayitas;                          //RAYA ACTUAL EN LA LECTURA
float tiempo;                           //VARIABLE AUXILIAR PARA GUARDAR LA DURACIÓN DE LA RAYA ACTUAL
float tiempo_total;                         //TIEMPO TOTAL DE LA TARJETA, USADO PARA DISTINGUIR RAYAS LARGAS Y CORTAS
float rayitas[NUM_TOTAL_RAYITAS];                 //VECTOR DONDE SE GUARDAN TIEMPOS
char traducidoh[NUM_TOTAL_RAYITAS];                 //VECTOR BINARIO DONDE DISTINGUIMOS ENTRE RAYAS LARGAS (0) Y CORTAS (1)
uint16_t aiuda;                           //UTILIZADO PARA ALGORITMO DE COMPARACIÓN Y ESTIMACIÓN. SE SACA A PARTIR DEL VECTOR traducidoh
uint16_t a[] = {0x0123, 0x0456, 0x0789, 0x0097, 0x0531, 0x0642};  //POSIBLES COMBINACIONES DE TARJETAS
uint16_t b[] = {123, 456, 789, 97, 531, 642};
int n = 6;                              //NÚMERO TOTAL DE TARJETAS POSIBLES. AUXILIAR.
int muchobueno = 0;                         //DETECTADA UNA TARJETA VÁLIDA
unsigned long int luz;                              //VARIABLE PARA ENCENDER EL LED DURANTE 1 SEGUNDO SIN BLOQUEOS

int whoMax(float *c) {    //DEVUELVE MEJOR COINCIDENCIA (MÁS PESO) SEGÚN EL ÍNDICE CALCULADO
  float max = c[0];
  int who = 0;
  for (int i = 1; i < n; i++) {
    if (c[i] > max) {
      who = i;
      max = c[i];
    }
  }
  return who;
}
float sumData(uint16_t c) {   //NÚMERO DE '1' EN EL DATO C. USADO PARA CALCULAR COINCIDENCIAS
  float howmuch = 0;
  for (int i = 0; i < 16; i++) {
    howmuch += rbi(c, i);
  }
  return howmuch;
}
void aiudado() {
  aiuda = 0;
  for (int i = 20; i > 8; i--) {
    if (traducidoh[i]) {
      aiuda  |= (1 << (20 - i));
    }
  }
  float jaccard[n];
  for (int j = 0; j < n; j++) {
    jaccard[j] = sumData( a[j] & aiuda) / sumData( a[j] | aiuda);
  }
  if ((jaccard[whoMax(jaccard)] > 0.99) || ((aiuda & 0x0FF) == 0x0FF)) { //Si hay 1 letra mayor que D, descartamos mucho malo. Si no, todo bien.
	serialPrint("\t Tarjeta detectada: ");
	serialPrintInt(b[whoMax(jaccard)]);
    //digitalWrite(S01, HIGH);
    tbi(OUTRUT, L1);
    luz = millis();
	muchobueno=1;
  }
  serialPrint("\n");
}

//PASAMOS DE UN VECTOR DE TIEMPOS A UN VECTOR QUE NOS DIGA QUÉ RAYAS SON CORTAS Y QUÉ RAYAS SON LARGAS
void Traduce() {
  float limite = 0;
  for (int i = 1; i < NUM_TOTAL_RAYITAS; i++) {
    limite = limite + rayitas[i];
  }
  limite = limite / (NUM_TOTAL_RAYITAS);
  for (int i = 1; i < NUM_TOTAL_RAYITAS - 1; i++) {
    if (rayitas[i] > limite) {
      traducidoh[i] = LRG;
    }
    else {
      traducidoh[i] = CRT;
    }
  }
  aiudado();
}

//VAMOS GUARDANDO LA DURACIÓN DE LAS RAYAS. ES POSIBLE QUE HAYA QUE PASAR A LA INTERRUPCIÓN EL ALMACENAMIENTO DE TIEMPOS PARA MEJORAR LA PRECISIÓN.
void cuenta() {
  if (bandera1 == 1 && bandera2 == 0) {     //HA ENTRADO UNA NUEVA RAYA
    tiempo = micros();
    if (num_rayitas == 1) tiempo_total = micros();
    bandera2 = 1;
    if (num_rayitas > 2 && rayitas[num_rayitas - 1] > 30000) num_rayitas = 0;
  }
  else if (bandera1 == 0 && bandera2 == 1) {  //SALIMOS DE LA RAYA ACTUAL
    tiempo = micros() - tiempo;
    bandera2 = 0;
    rayitas[num_rayitas] = tiempo;
    if (num_rayitas == NUM_TOTAL_RAYITAS - 2) {
      tiempo_total = micros() - tiempo_total;
      Traduce();
    }
    num_rayitas++;
    if (num_rayitas == NUM_TOTAL_RAYITAS) num_rayitas = 0;
  }
}
  ISR(SO1_vect) {
    bandera1 = !bandera1;
  }


  void setup() {
	//pinMode(SO1, INPUT);
    sbi(INT_MASK, SO1);
	//pinMode(L1, OUTPUT);
	sbi(DDR_OUTRUT,L1);
	cbi(OUTRUT, L1);
	//COMENTAR initTime AL PASAR AL PROGRAMA CONJUNTO!!!!!!!!!!!!!!!!!!
	initTime();
	serialBegin(9600);
    bandera1 = 0;
    bandera2 = 0;
    num_rayitas = 0;
    tiempo_total = 0;
	muchobueno=0;
    //attachInterrupt(digitalPinToInterrupt(S01), []() {bandera1 = !bandera1;}, CHANGE);
    sbi(CTRL_INT, SO1_C0);
    cbi(CTRL_INT, SO1_C1);
	bandera1=millis();
  }

  int main(void) {
    setup();
    while (1) {
      cuenta();
      if (muchobueno && ((millis() - luz) > 1000)) {
        tbi(OUTRUT, L1);
		muchobueno = 0;
      }
    }
  }