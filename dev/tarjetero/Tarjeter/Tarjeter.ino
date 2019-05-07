/*********************************
 *        Tarjeter.ino          *
 * ------------------------------*
 *                               *
 * Laura Aguado Cabanillas 15003 *
 * Héctor Gutiérrez Li     15202 *
 *                               *
 **********************************/

#include "Functions.h"

#define INTERRUPT_PIN 3
#define MAX 26

int   Counter=0;
float Timer=0;
float Times[MAX];
float Length[MAX-2];


void width() {
  float lim = 0;
  lim = (Times[6] + Times[7] + Times[8] + Times[9] + Times[21] + Times[22] + Times[23] + Times[24]) / 8; //Media
  //Distinguish length
  for (int i = 1; i < MAX - 1; i++) {
    Length[i - 1] = (Times[i] > lim);
  }
  Serial.print("-----------------CODIGO-----------------");
  for (int i = 0; i < MAX - 2; i += 4) { //Imprimir codigo
    Serial.print(code(Length[i], Length[i + 1], Length[i + 2], Length[i + 3]));
  }
  Serial.println("");
}

void kun() {
  if (!digitalRead(INTERRUPT_PIN))
    Timer = micros();
  else {
    Timer = micros() - Timer;
    Serial.println(Timer);
  }
  Times[Counter] = Timer;
  Counter++;
}

void setup() {
  interrupt_setup();
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), kun, CHANGE);
  Serial.begin(9600);
}

void loop() {
  if(Counter==MAX){
    width();
    Counter = 0;
  }
}
