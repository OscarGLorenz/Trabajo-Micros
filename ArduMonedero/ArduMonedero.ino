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
uint8_t width_SO2, width_SO3, delay_SO, total_SO;

struct ratios {
  float w3_d;
  float w3_t;
};

struct coin_params { 
  ratios mins;
  ratios maxs;
};

uint8_t new_coin;
uint8_t configure, configure_coin;

// Coin characterization   1c      2c      10c     5c      20c     50c     1e      2e
coin_params coins[8];
uint8_t coin_id;
// float ratiosW3_d[8] = {}; //= {0.40,   0.45,   0.50,   0.60,   0.80,   0.90,   1.00,   2.20,   2.60};
// float ratiosW3_t[8] = {};

// Interrupt Service Routines ######################################

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    time_SO2 = millis();
    new_coin = false;
  }
  else {
    width_SO2 = uint8_t(millis() - time_SO2);
  }
}

void ISR_SO3(){   // ISR of second optic sensor
  if (digitalRead(SO3)){
    time_SO3 = millis();
  }
  else {
    width_SO3 = uint8_t(millis() - time_SO2);
    new_coin = true;
  }
}

// Functions #######################################################

void serialWatchdog(){
  if (Serial.available()){
    String command = Serial.readString();
    if (command.equalsIgnoreCase("CONFIGURE")) configure = 1;
    else if (command.equalsIgnoreCase("RUN")) configure = 0;
    else {
      configure_coin = command.toInt();
      switch(configure_coin) {
        case 1: coin_id = 0; break;
        case 2: coin_id = 1; break;
        case 5: coin_id = 2; break;
        case 10: coin_id = 3; break;
        case 20: coin_id = 4; break;
        case 50: coin_id = 5; break;
        case 100: coin_id = 6; break;
        case 200: coin_id = 7; break;
      }
    }
  }
}

void newCoin(){    // Compares ratio with valid ranges
  uint8_t delay_SO = uint8_t(time_SO3 - time_SO2);
  uint8_t total_SO = delay_SO + width_SO3;
  ratios coin_ratios;
  coin_ratios.w3_d = float(width_SO3) / delay_SO;
  coin_ratios.w3_t = float(width_SO3) / total_SO;
  if (configure) adjustCoin(coin_ratios);
  else compareCoin(coin_ratios);
}

void adjustCoin(ratios coin_ratios){
  
}

void compareCoin(ratios coin_ratios){
  uint8_t cents;
  #ifdef DEBUG
      Serial.println("New coin detected, ratio is " + String(coin_ratios.w3_d));
    #endif
    /*
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
  */
  if ((cents > 5) && (cents < 200)){    // Accepted coins condition
    coinAccepted(cents);
  }
}

void coinAccepted(uint8_t cents){   // Acepts coin if ratio was validated
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
  openCollector();  
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

  Serial.begin(9600);
  
  digitalWrite(M1_bk, HIGH);

  configure = false;
  new_coin = false;
  payment_money = 0;
  queue = 0;
}

void loop() {  // ##################################################
  serialWatchdog();
  if (new_coin){
    newCoin();
  }
}
