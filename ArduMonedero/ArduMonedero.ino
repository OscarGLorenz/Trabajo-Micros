// Main file for coin collector

// Precompiler definitions #########################################
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin assignment in "pindef.h"
#define DEBUG         // Uncomment for serial debugging

// Global variables ################################################

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables
unsigned long int time_SO2, time_SO3;
uint8_t width_SO2, width_SO3, delay_SO;
float ratio_SO;
uint8_t pending_coin;

// Coin characterization           1c      2c      10c     5c      20c     50c     1e      2e
const float ratio_limits[] = {0.40,   0.45,   0.50,   0.60,   0.80,   0.90,   1.00,   2.20,   2.60};

// Interrupt Service Routines ######################################

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    time_SO2 = millis();
  }
  else {
    width_SO2 = uint8_t(millis() - time_SO2);
  }
}

void ISR_SO3(){   // ISR of second optic sensor
  if (digitalRead(SO3)){
    time_SO3 = millis();
    delay_SO = uint8_t(time_SO3 - time_SO2);
  }
  else {
    width_SO3 = uint8_t(millis() - time_SO2);
    ratio_SO = float(width_SO3) / delay_SO;
    #ifdef DEBUG
      Serial.println("New coin detected, ratio is " + String(ratio_SO));
    #endif
    newCoin(ratio_SO);
  }
}

// Functions #######################################################

void newCoin(float ratio){    // Compares ratio with valid ranges
  uint8_t cents;
  if ((ratio < ratio_limits[0]) || (ratio > ratio_limits[8])) {
    cents = 0;                // Object detected out of range
  }
  else if ((ratio >= ratio_limits[0]) && (ratio < ratio_limits[1])) {
    cents = 1;                // Object detected as 1c coin
  }
  else if ((ratio >= ratio_limits[1]) && (ratio < ratio_limits[2])) {
    cents = 2;                // Object detected as 2c coin
  }
  else if ((ratio >= ratio_limits[2]) && (ratio < ratio_limits[3])) {
    cents = 10;               // Object detected as 10c coin
  }
  else if ((ratio >= ratio_limits[3]) && (ratio < ratio_limits[4])) {
    cents = 5;                // Object detected as 5c coin
  }
  else if ((ratio >= ratio_limits[4]) && (ratio < ratio_limits[5])) {
    cents = 20;               // Object detected as 20c coin
  }
  else if ((ratio >= ratio_limits[5]) && (ratio < ratio_limits[6])) {
    cents = 50;               // Object detected as 50c coin
  }
  else if ((ratio >= ratio_limits[5]) && (ratio < ratio_limits[7])) {
    cents = 100;              // Object detected as 100c coin
  }
  else if ((ratio >= ratio_limits[5]) && (ratio < ratio_limits[8])) {
    cents = 200;              // Object detected as 200c coin
  }
  
  if ((cents > 5) && (cents < 200)){    // Accepted coins condition
    coinAccepted(cents);
  }
}

void coinAccepted(uint8_t cents){   // Acepts coin if ratio was validated
  pending_coin = true;    // Enable coin accepted flag to control rejection motor
  payment_money += cents;     // Add coin value to money of payment in progress
  #ifdef DEBUG            // Debug money introduced
    Serial.print("New coin accepted: " + String(cents));
    Serial.println(", inserted money: " + String(payment_money));
  #endif
  if (payment_money >= 120){
    queue++;
    #ifdef DEBUG            // Debug people in queue
      Serial.print("New payment completed: " + String(payment_money));
      Serial.println(", people in queue: " + String(queue));
    #endif
    payment_money = 0;
  }
}

void wait(int milliseconds){
  unsigned long int prev_time = millis();
  while ((millis() - prev_time) < milliseconds){
  }
}

void openCollector(){
  unsigned long int prev_time;
  digitalWrite(M1_en, HIGH);    // Open coin slot
  while (digitalRead(SW2)){
  }
  wait(10);                     // Substitute with timer interrupt!!
  digitalWrite(M1_en, LOW);     // Wait for coin to pass thorugh
  wait(100);
  digitalWrite(M1_en, HIGH);    // Close coin slot
  while (!digitalRead(SW2)){    
  }
  wait(10);
  digitalWrite(M1_en, LOW);     // Keep in rejection position
  pending_coin = false;
}

void setup() {  // #################################################
  pinMode(SO2, INPUT);
  pinMode(SO3, INPUT);
  pinMode(SW2, INPUT);
  pinMode(M1_en, OUTPUT);
  pinMode(M1_en, OUTPUT);
  pinMode(L2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO3, CHANGE);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  digitalWrite(M1_bk, HIGH);
  
  pending_coin = false;
  payment_money = 0;
  queue = 0;
}

void loop() {  // ##################################################
  if (pending_coin){
    openCollector();
  }
}
