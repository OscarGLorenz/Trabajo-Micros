/***********************************************************************************
*
* FICHERO: tarjetero.c
*
* DESCRIPCIÓN :
* 	Contiene la funcionalidad del tarjetero. Implementada en 3 fases:
*
*	1 - Obtención de datos: en la interrupción guardamos el tiempo
*       en un vector
*   2 - Análisis de datos: comparamos los tiempos de cada intervalo para
*       distinguir si es una raya larga o corta.
*   3 - Comparación con patrones: cotejar los datos obtenidos 
*       con las posibles tarjetas. En caso de una coincidencia mayor al 75%
*       aceptamos la tarjeta y se lo notificamos al programa principal.
*		También se enciende el led durante 1 segundo
*
* AUTORES:
*
* 	Laura Aguado Cabanillas	
*	
*   Héctor Gutiérrez Li
*
***********************************************************************************/

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
//Callback de nueva persona
static void (*callback) ();
// Variables globales deben ser estáticas
static unsigned long flancos[61];  //Vector para guardar tiempos segun el flanco. Son 52 flancos y anadimos 2 por falsso flancos al inicio
static unsigned long rayas[27];    //Vector con la duracion de cada rayas. Son 26 rayas y anadimos 1 por falsos flancos al inicio
static unsigned binario[27];    //Para tener la tarjeta en binario. Son 26 rayas y anadimos 1 por falsos flancos al inicio
static int flanco_actual;     //Vamos contando el numero de flancos que vamos detectando
static int traduce;         //Bandera para empezar a traducir
static uint16_t hexadecimal;
//Patrones de tarjetas
static uint16_t a[] = {0x1230, 0x4560, 0x7890, 0x0970, 0x5310, 0x6420};  //POSIBLES COMBINACIONES DE TARJETAS
static uint8_t abono[] = {1, 2, 3, 4, 5, 6}; 
static uint16_t b[] = {123, 456, 789, 97, 531, 642};
//Variables de control de led
static int luz;
static int encendido;
static int n;
static int falsoFlanco;
//Variables auxiliares para división entera en ensamblador
static uint8_t num;
static uint8_t den;
//Bandera de funcionamiento normal o parada de emergencia
static funciona = 1;


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
//Devuelve mejor coincidencia (más peso) según el índice calculado
uint8_t whoMax(uint8_t *c) {    
  uint8_t max = c[0];
  uint8_t who = 0;
  for (int i = 1; i < n;  i++) {
    if (c[i] > max) {
      who = i;
      max = c[i];
    }
  }
  return who;
}
//Número de '1' en el dato C. Se usa para calcular coincidencias
uint8_t sumData(uint16_t c) {   
  uint8_t howmuch = 0;
  for (int i = 0; i < 16; i++) {
    howmuch += rbi(c, i);
  }
  return howmuch;
}
//Algoritmo de comparación
void tanimoto() {
  uint8_t jaccard[n];
  serialPrint("Pesos jaccard \t");
  for (int j = 0; j < n; j++) {
	  num = sumData(a[j] & hexadecimal);
	  den = (sumData(a[j] | hexadecimal));
	  //Multiplicación del numerador x4 y división entera en ensamblador
	  asm volatile (
    "push r17     \n"		//numerador
    "push r19     \n"		//denominador
    "push r16     \n"		//auxiliar
    "mov r17, %1  \n"
    "mov r19, %2  \n"
    "tst r19      \n"		//Comprobar si denominador igual a 0
    "breq zero    \n"
	"tst r17      \n"
	"breq zero	  \n"
    "clr %0       \n"		//Poner cociente a 0
	"lsl r17	  \n"		//Incrementar valor de numerador para hacer division entera (x2)
	"lsl r17	  \n"		//(x4)
    "mov r16,r19  \n"		//Inicializar auxiliar con denominador
    "start:       \n"
    " cp r17, r16 \n"		
    " brge loop   \n"		//Salir cuando numerador menor que auxiliar
    " jmp end     \n"
    "loop:        \n"		//Incrementar valor auxiliar hasta que sea mayor o igual al numerador
    " inc %0      \n"		//Incrementar cociente en 1
    " add r16,r19 \n"		
    " jmp start   \n"		
    "zero:        \n"		//Caso en que el denominador es igual a 0
    " clr %0      \n"		//Cociente es 0
    "end:         \n"
    " pop r16      \n"
    " pop r19      \n"
    " pop r17      \n"
    :"=r"(jaccard[j])		//Salida
	:"r"(num),"r"(den)		//Entradas (numerador, denominador)
  );
    //jaccard[j] = sumData(a[j] & hexadecimal) / (float(sumData(a[j] | hexadecimal)));
	serialPrintFloat(jaccard[j]);
    serialPrint("\t");
  }
  flanco_actual = 0;
  if ((jaccard[whoMax(jaccard)] >= 3)) { //Si hay 1 letra mayor que D, descartamos mucho malo. Si no, todo bien.  Si se usa ensamblador, limite=3. Sin ensamblador, 0.75
    serialPrint("\t Tarjeta detectada: ");
    serialPrintInt(b[whoMax(jaccard)]);
	if(abono[whoMax(jaccard)]>0){
	abono[whoMax(jaccard)] = abono[whoMax(jaccard)]-1;
	serialPrint("Viajes restantes: ");
	serialPrintInt(abono[whoMax(jaccard)]);
	serialPrint("\n");
    //digitalWrite(S01, HIGH);
    sbi(OUTRUT, L1);
    luz = millis();
    encendido = 1;
	callback();
	}
	else{
		serialPrint("No quedan mas viajes\n");
	}
  }
  else{
  serialPrint("\t Tarjeta rechazada");
  }
  serialPrint("\n");
}
//Pasar de un vector binario a un entero según el código de la tarjeta en hexadecimal
void traduceHex() {
  //Comprobamos que la tarjeta es válida
  hexadecimal = 0;
  for (int i = 10; i < 22; i++) {     //falso flanco
    if (binario[i]) {           //Sumamos 1 para evitar el falso flanco
      hexadecimal |= (1 << (25 - i));   //hexadecimal = 25-rayas[i];
    }
  }
  hexadecimal &= 0xFFF0;
  tanimoto();
}
//Pasar de tiempos a rayas largas o cortas
void traduceBin() {
  //int largo = 0;
  long limite = (rayas[25] + rayas[24] + rayas[23] + rayas[22] + rayas[7] + rayas[8] + rayas[9] + rayas[10]) / 8; //8 en vez de 3 por falso flanco al inicio
    for (int i = 2; i < 26; i++) {
      if (rayas[i] > (unsigned long int) limite) {
        binario[i] = LRG;
      }
      else {
        binario[i] = CRT;
      }
    }
	traduceHex();
}

void tarjeteroLoop(){
	if(funciona){
		if (traduce) {
		  traduceBin();
		  traduce = 0;
		}
		//Apagar el led cuando corresponda
		if (encendido && (millis() - luz >= 1000)) {
		  cbi(OUTRUT, L1);
		  encendido = 0;
		}
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
  //Iniciamos variables
  traduce = 0;
  encendido = 0;
  flanco_actual = 0;
  n = 6;
  falsoFlanco= 0;
}
//Callback de nueva persona que entra
void tarjeteroSetCallbackCorrecto(void(*f)()) {
	callback = f;
}
//Función para parada de emergencia (que el tarjetero deje de funcionar)
void tarjeteroParar() {
	funciona = 0;
	cbi(OUTRUT, L1);
	encendido = 0;
	serialPrint("Tarjetero deshabilitado\n");
}