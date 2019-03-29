// Main file for coin collector



// Precompiler definitions #########################################
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin definition in "pindef.h"
#define DEBUG         // Uncomment for serial debugging

// Global variables ################################################
uint8_t queue;
uint8_t paymentMoney;

// Monedas: 10c, 20c, 50c, 1e
const float ratio_limits[] = {0.5, 0.8, 0.9, 1.0, 2.2};

uint8_t time_SO2, time_SO3, width_SO2, width_SO3, delay_SO;
float ratio_SO;

// Interrupt Service Routines ######################################

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    time_SO2 = millis();
  }
  else {
    width_SO2 = millis() - time_SO2;
  }
}

void ISR_SO3(){   // ISR of second optic sensor
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


// Functions #######################################################

void newCoin(float ratio){    // Compares ratio with valid ranges
  uint8_t cents;
  
  coinAccepted(cents);
}

void coinAccepted(uint8_t cents){   // Acepts coin if ratio was validated
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

void paymentAccepted(){   // New payment completed, add 1 to queue
  paymentMoney = 0;
  queue++;
}

void setup() {  // #################################################
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

void loop() {  // ##################################################
}
