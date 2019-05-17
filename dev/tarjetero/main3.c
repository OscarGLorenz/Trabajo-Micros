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
static int flanco_actual;	//Vamos contando el numero de flancos que vamos detectando
static int flanco_traducido;
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

void traduceatiempos() {
	if(flanco_actual > flanco_traducido){
		if((flanco_traducido%2)){
			rayas[(flanco_traducido-1)/2] = flancos[flanco_actual]-flancos[flanco_actual-1];
		}
		flanco_traducido+=2;
	}
	if(flanco_traducido>= 52){
		flanco_traducido=0;
		serialPrint("Mucho malo tarjeto\n");
	}
}

void traduceabin(){
	int largo = 0;
	long limite = (rayas[3]+rayas[4]+rayas[5]+rayas[6])/4;
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
		traduceatiempos();
	}
}