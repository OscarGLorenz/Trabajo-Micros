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

static unsigned long flancos[52];	//Vector para guardar tiempos segun el flanco
static unsigned long rayas[26];		//Vector con la duracion de cada rayas
static unsigned binario[26];		//Para tener la tarjeta en binario
static int flanco_actual;			//Vamos contando el numero de flancos que vamos detectando
static int flanco_traducido;		//Primer flanco de la raya que estamos traduciendo
static uint16_t hexadecimal;
static uint16_t a[] = {0x1230, 0x4560, 0x7890, 0x0970, 0x5310, 0x6420};  //POSIBLES COMBINACIONES DE TARJETAS
static uint16_t b[] = {123, 456, 789, 97, 531, 642};
static int luz;
static int encendido;
ISR(S01_vect) {
	flancos[flanco_actual] = micros();
	flanco_actual++;
}

void corrige(){ //Para evitar falsos flancos o si se pasa mal la tarjeta
	if((!(flanco_actual>=3)) && ((flancos[3]-flanco[2]) > 30000)||((flancos[1]-flanco[0]) > 30000)){
		flanco_actual = 0;
		flanco_traducido = 0;
	}
}

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

void traduceTiempos() {
	if(flanco_actual > flanco_traducido){
		if(flanco_actual){
			rayas[(flanco_traducido)/2] = flancos[flanco_traducido+1]-flancos[flanco_traducido];
		}
		flanco_traducido+=2;
	}
	if(flanco_traducido>= 51){
		flanco_traducido=0;
	}
}

void tanimoto(){
	float jaccard[n];
	for (int j = 0; j < n; j++) {
		jaccard[j] = sumData( a[j] & hexadecimal) / sumData( a[j] | hexadecimal);
	}
	if ((jaccard[whoMax(jaccard)] > 0.8) || ((hexadecimal & 0x0FF) == 0x0FF)) { //Si hay 1 letra mayor que D, descartamos mucho malo. Si no, todo bien.
		serialPrint("\t Tarjeta detectada: ");
		serialPrintInt(b[whoMax(jaccard)]);
		//digitalWrite(S01, HIGH);
		tbi(OUTRUT, L1);
		luz = millis();
		encendido=1;
	}
  serialPrint("\n");
}

void traduceBin(){
	int largo = 0;
	long limite = (rayas[3]+rayas[4]+rayas[5]+rayas[6]+rayas[7])/5;
	for (int i = 1; i<25; i++){
		if(!largo){
			if(rayas[i]<limite*1.2){
				binario[i] = CRT;
			}
			else{
				binario[i] = LRG;
				largo = 1;
				limite = rayas[i];
			}
		}
		else{
			if(rayas[i]<limite*0.8){
				binario[i] = CRT;
				largo = 0;
				limite = rayas[i];
			}
			else{
				binario[i] = LRG;
			}
		}
	}
	traduceHex();
}

void traduceHex(){
	//Comprobamos que la tarjeta es válida (no se pasa al revés)
	if(rayas[0]<15000 || (rayas[24] ==CRT && rayas[23] ==CRT&&rayas[22] ==CRT&&rayas[21] ==CRT)){
		hexadecimal = 0xFFFF;
	}else{
		for(int i = 9; i<21; i++){
			if(rayas[i]){
				hexadecimal|= (1 << (24 - i));		//hexadecimal = 24-rayas[i];
			}
		}
		hexadecimal&=0xFFF0;
	}
	tanimoto();
}

void setup(){
	//pinMode(S01, INPUT); Pin del sensor como entrada
	sbi(INT_MASK, S01);
	//pinMode(L1, OUTPUT); Pin del led como salida
	sbi(DDR_OUTRUT, L1);
	//attachInterrupt(digitalPinToInterrupt(S01,), Interrupcion, CHANGE);
	sbi(CTRL_INT, SO1_C0);
	cbi(CTRL_INT, SO1_C1);
	//digitalWrite(L1, LOW); Poner led apagado
	cbi(OUTRUT, L1);
	//Iniciamos timer para millis() y micros()
	initTime();
	serialBegin(9600);
}

int main(void){
	setup();
	for(;;){
		corrige();
		traduceTiempos();
		if(flanco_traducido>= 50){
			traduceBin();
		}
		if(encendido && (millis()-luz == 1000)){
			tbi(OUTRUT, L1);
			encendido = 0;
		}
	}
}