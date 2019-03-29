#include <Arduino.h>
#include "macros.h"

#define DEBUG

// Defines #########################################################

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

// Global variables #################################################
int queue;
int paymentMoney;

// Monedas: 10c, 20c, 50c, 1e
const float ratio_limits[] = {0.5, 0.8, 0.9, 1.0, 2.2};

uint8_t time_SO2, time_SO3, width_SO2, width_SO3, delay_SO;
float ratio_SO;

// Interrupt Service Routines #######################################

void ISR_SO2(){
  if (digitalRead(SO2)){
    time_SO2 = millis();
  }
  else {
    width_SO2 = millis() - time_SO2;
  }
}

void ISR_SO3(){
  if (digitalRead(SO3)){
    time_SO3 = millis();
    delay_SO = time_SO3 - time_SO2;
  }
  else {
    width_SO3 = millis() - time_SO2;
    ratio_SO = float(width_SO3) / delay_SO;
    
    #ifdef DEBUG
    Serial.println("New coin detected, ratio is " + String(ratio_SO) );
    #endif
    
    newCoin(ratio_SO);
  }
}


// Functions #########################################################

void newCoin(float ratio){
  
}

void coinAccepted(int cents){
  paymentMoney += cents;
  if (paymentMoney >= 120){
    paymentAccepted();
  }
  // Motor control to enable coin accepted


  // Debug money introduced
  #ifdef DEBUG
    Serial.print("New coin accepted: " + String(cents) );
    Serial.println(", inserted money: " + String(paymentMoney) );
  #endif
  
}

void paymentAccepted(){
  paymentMoney = 0;
  queue++;
}

void setup() {  // ###################################################
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
  paymentMoney = 0;
}

void loop() {  // ####################################################
}
