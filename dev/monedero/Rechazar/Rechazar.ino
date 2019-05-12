
// Precompiler definitions #########################################
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin assignment in "pindef.h
#define DEBUG         // Uncomment for serial debugging
#include "default_coin_limits.h"  // Load pre-calibrated limits for coin detection

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
unsigned long t2u, t3u, t2d, d, s;
unsigned long t;

//Flags ------------------------------
uint8_t coin_state;
uint8_t cerrado = 0;


// Interrupt Service Routines ######################################

void ISR_SO2() {  // ISR of first optic sensor
  if (digitalRead(SO2)) {
    if (coin_state == 0) {
      t2u = micros();
      coin_state = 1;
    }
    else {
      coin_state = 0;
    }
  }
  else if (coin_state == 2) {
    t2d = micros();
    coin_state = 3;
  }
  else {
    coin_state = 0;

  }
}

void ISR_SO3() {  // ISR of second optic sensor
  if (digitalRead(SO3)) {
    if (coin_state == 1) {
      t3u = micros();
      coin_state = 2;
    }
    else {
      coin_state = 0;

    }
  }
  else if (coin_state == 3) {
    coin_state = 4;

  }
  else {
    coin_state = 0;

  }
}

void newCoin() {   // Compares ratio with valid ranges
  d = t3u - t2u;
  s = t2d - t3u;
  float ratio;
  ratio = float(d) / s;

#ifdef DEBUG
  Serial.println("\t" + String(d) + "\t" + "\t" + String(s));
#endif
  Serial.println(ratio);
  compareCoin(ratio);
}

void compareCoin(float ratio) {  // Cambiar lo que se compara en casos en los que se solapen algunos ratios
  uint8_t cents;

  if ((ratio >= coins[1].min) && (ratio <= coins[1].max)) cents = 2;     // Object detected as 2c coin
  else if ((ratio >= coins[2].min) && (ratio <= coins[2].max)) cents = 5;     // Object detected as 5c coin
  else if ((ratio >= coins[3].min) && (ratio <= coins[3].max)) cents = 10;    // Object detected as 10c coin
  else if ((ratio >= coins[4].min) && (ratio <= coins[4].max)) cents = 20;    // Object detected as 20c coin
  else if ((ratio >= coins[5].min) && (ratio <= coins[5].max)) cents = 50;    // Object detected as 50c coin
  else if ((ratio >= coins[6].min) && (ratio <= coins[6].max)) cents = 100;   // Object detected as 100c coin
  else if ((ratio >= coins[7].min) && (ratio <= coins[7].max)) cents = 200;   // Object detected as 200c coin
  else if ((ratio > coins[2].max) && (ratio < coins[7].min)) cents = 1;     // Object detected as 1c coin
  else {            // Object detected out of range
    cents = 0;
  }
  if (cents > 5 && cents < 200) {
    cerrado = 1;
    t = millis();
    barrera();
    Serial.print(cents);;
    coinAccepted(cents);
  }
}

void coinAccepted(uint8_t cents) {  // Acepts coin if ratio was validated
  payment_money += cents;     // Add coin value to money of payment in progress
  Serial.print(payment_money);
  if (payment_money >= 120) {
    queue++;

    Serial.print(payment_money);
    Serial.print(queue);
    payment_money = 0;
  }
}

void barrera(){
    if (cerrado) {
    if (!digitalRead(SW2)) {
      arranca();
    }
    else {
      para();
    }
  }
  else{
    if (digitalRead(SW2)) {
      arranca();
    }
    else {
      para();
    }
  }
}

void arranca() {
  digitalWrite(M1_bk, 0);

}
void para() {
  digitalWrite(M1_bk, 1);
}

void setup() {
  pinMode(SO2, INPUT);
  pinMode(SO3, INPUT);
  pinMode(SW2, INPUT);
  pinMode(M1_bk, OUTPUT);
  pinMode(L2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SO3), ISR_SO3, CHANGE);

  loadDefaultLimits();

  Serial.begin(9600);

  coin_state = 0;
  payment_money = 0;
  queue = 0;
}

void loop() {
  if (coin_state == 4) {
    coin_state = 0;
    newCoin();
  }
  
 barrera();
 
 if(cerrado && ((millis()-t)>1000))
  cerrado = 0;
}
