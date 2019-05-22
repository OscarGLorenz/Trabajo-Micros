#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"

#include "tarjetero.h"
#include <stdint.h>



#define LRG 0           //DEFINIMOS RAYAS LARGAS CON CÓDIGO 0
#define CRT 1           //DEFINIMOS RAYAS CORTAS CON CÓDIGO 1
#define HIGH 1
#define LOW 0
// Variables globales deben ser estáticas
static void (*callback) ();

static unsigned long flancos[61];  //Vector para guardar tiempos segun el flanco. Son 52 flancos y anadimos 2 por falsso flancos al inicio
static unsigned long rayas[27];    //Vector con la duracion de cada rayas. Son 26 rayas y anadimos 1 por falsos flancos al inicio
static unsigned binario[27];    //Para tener la tarjeta en binario. Son 26 rayas y anadimos 1 por falsos flancos al inicio
static int flanco_actual;     //Vamos contando el numero de flancos que vamos detectando
static int traduce;         //Bandera para empezar a traducir
static uint16_t hexadecimal;
static uint16_t a[] = {0x1230, 0x4560, 0x7890, 0x0970, 0x5310, 0x6420};  //POSIBLES COMBINACIONES DE TARJETAS
static uint16_t b[] = {123, 456, 789, 97, 531, 642};
static int luz;
static int encendido;
static int n;
static int falsoFlanco;

ISR(SO1_vect) {
    flancos[flanco_actual] = micros();
    if (flanco_actual % 2) {
      rayas[(flanco_actual) / 2] = flancos[flanco_actual] - flancos[flanco_actual - 1];
    }
    if (flanco_actual >= 53 - falsoFlanco) {
		if(flanco_actual>=60){
			flanco_actual = -1;
		}
		falsoFlanco = 0;
      traduce = 1;
	}
	if(flanco_actual>0){
		if((flancos[flanco_actual] - flancos[flanco_actual-1])>200000){
			flancos[0] = flancos[flanco_actual];
			flanco_actual = -1;
			falsoFlanco=0;
		}
	}
	if (flanco_actual == 3) {
      if (rayas[0] > rayas[1]) {
        rayas[2] = rayas[1];
        rayas[1] = rayas[0];
        rayas[0] = 0;
		flancos[4] = flancos[2];
		flancos[5] = flancos[3];
        flanco_actual += 2;
		falsoFlanco = 1;
      }
    }
    flanco_actual++;
}

/*void corrige(){ //Para evitar falsos flancos o si se pasa mal la tarjeta
  if(((flancos[flanco_actual]-flancos[flanco_actual-1])>10000000) && (flanco_actual>1)){
    flanco_actual = 0;
  }
  }*/

int whoMax(float *c) {    //DEVUELVE MEJOR COINCIDENCIA (MÁS PESO) SEGÚN EL ÍNDICE CALCULADO
  float max = c[0];
  int who = 0;
  for (int i = 1; i < n;  i++) {
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
void tanimoto() {
  float jaccard[n];
  serialPrint("Pesos jaccard \t");
  for (int j = 0; j < n; j++) {
    jaccard[j] = sumData(a[j] & hexadecimal) / (float(sumData(a[j] | hexadecimal)));
    serialPrintFloat(jaccard[j]);
    serialPrint("\t");
  }
  flanco_actual = 0;
  if ((jaccard[whoMax(jaccard)] >= 0.75)) { //Si hay 1 letra mayor que D, descartamos mucho malo. Si no, todo bien.
    serialPrint("\t Tarjeta detectada: ");
    serialPrintInt(b[whoMax(jaccard)]);
    //digitalWrite(S01, HIGH);
    sbi(OUTRUT, L1);
    luz = millis();
    encendido = 1;
	callback();
  }
  serialPrint("\n");
}

void traduceHex() {
  //Comprobamos que la tarjeta es válida (no se pasa al revés)
  hexadecimal = 0;
  for (int i = 10; i < 22; i++) {     //falso flanco
    if (binario[i]) {           //Sumamos 1 para evitar el falso flanco
      hexadecimal |= (1 << (25 - i));   //hexadecimal = 25-rayas[i];
    }
  }
  hexadecimal &= 0xFFF0;
  serialPrint("Traducido a hex:");
  serialPrintULong(hexadecimal);
  serialPrint("\n");
  tanimoto();
}

void traduceBin() {
  serialPrint("Bin ");
  //int largo = 0;
  long limite = (rayas[25] + rayas[24] + rayas[23] + rayas[22] + rayas[7] + rayas[8] + rayas[9] + rayas[10]) / 8; //8 en vez de 3 por falso flanco al inicio
    for (int i = 2; i < 26; i++) {
      if (rayas[i] > (unsigned long int) limite) {
        binario[i] = LRG;
        serialPrintInt(LRG);
      }
      else {
        binario[i] = CRT;
        serialPrintInt(CRT);
      }
    }
	serialPrint("\n");
	traduceHex();
  /*for (int i = 2; i<26; i++){                     //Desplazado 1 por falsos flancos al inicio
    if(!largo){
      if(rayas[i]<limite*1.1){
        binario[i] = CRT;
        serialPrint("1");
      }
      else{
        binario[i] = LRG;
        largo = 1;
        serialPrint("0");
        limite = rayas[i];
      }
    }
    else{
      if(rayas[i]<limite*0.9){
        binario[i] = CRT;
        serialPrint("1");
        largo = 0;
        limite = rayas[i];
      }
      else{
        binario[i] = LRG;
        serialPrint("0");
      }
    }
    }*/
}

void tarjeteroLoop(){
	if (traduce) {
      traduceBin();
      traduce = 0;
    }
    if (encendido && (millis() - luz >= 1000)) {
      cbi(OUTRUT, L1);
      encendido = 0;
    }
}
void tarjeteroSetup() {
  //pinMode(S01, INPUT); Pin del sensor como entrada
  sbi(INT_MASK, SO1);
  //pinMode(L1, OUTPUT); Pin del led como salida
  sbi(DDR_OUTRUT, L1);
  //attachInterrupt(digitalPinToInterrupt(S01,), Interrupcion, CHANGE);
  sbi(CTRL_INT, SO1_C0);
  cbi(CTRL_INT, SO1_C1);
  //digitalWrite(L1, LOW); Poner led apagado
  cbi(OUTRUT, L1);
  //Iniciamos timer para millis() y micros()

  traduce = 0;
  encendido = 0;
  flanco_actual = 0;
  n = 6;
  falsoFlanco= 0;
}

void tarjeteroSetCallbackCorrecto(void(*f)()) {
	callback = f;
}

void tarjeteroParar() {

}