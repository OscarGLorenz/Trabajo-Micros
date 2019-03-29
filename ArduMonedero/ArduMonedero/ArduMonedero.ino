
#include <Arduino.h>

// Naming > connector

#define SO2 PIN_21
#define SO3 PIN_23
#define SW2 PIN_31
#define M1_en PIN_26
#define M1_bk PIN_24  
#define L2  PIN_22

// Connector > Arduino

#define PIN_21  2
#define PIN_23  3
#define PIN_31  4
#define PIN_26  5
#define PIN_24  6
#define PIN_22  7

int queue;
float money;

void ISR_SO2(){
  
}

void ISR_SO3(){
  
}

void setup() {
  Serial.begin(9600);
  
  pinMode(SO2, INPUT);
  pinMode(SO3, INPUT);
  pinMode(SW2, INPUT);
  pinMode(M1_en, OUTPUT);
  pinMode(M1_en, OUTPUT);
  pinMode(L2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO3, CHANGE);

  queue = 0;
  money = 0;
}

void loop() {
  
}
