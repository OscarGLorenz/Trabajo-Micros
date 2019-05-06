// Main file for coin collector

// Precompiler definitions #########################################
//#include "time.h"
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin assignment in "pindef.h
//#include "serial.h"

#include "default_coin_limits.h"  // Load pre-calibrated limits for coin detection
//#define DEBUG         // Uncomment for serial debugging

// Global variables ################################################

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
uint8_t t2, t3, w2, w3;

//Flags ------------------------------
uint8_t new_coin;
uint8_t calibrate;
uint8_t coin_id;

// Interrupt Service Routines ######################################

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    t2 = millis();
    new_coin = false;
    #if DEBUG
      Sprintln("Entra SO2"); 
    #endif
  }
  else {
    w2 = millis() - t2;
    #if DEBUG
      Sprintln("Sale SO2"); 
    #endif
  }
}

void ISR_SO3(){   // ISR of second optic sensor
  if (digitalRead(SO3)){
    t3 = millis();
    #if DEBUG
      Sprintln("Entra SO3"); 
    #endif
  }
  else {
    w3 = millis() - t3;
    new_coin = true;
    #if DEBUG
      Sprintln("Sale SO3");
    #endif
  }
}

// Functions #######################################################

void serialWelcome(){
  Sprintln("Available commands:");
  Sprintln("CALIB - Calibrate ratios limits for specific coin type");
  Sprintln("RUN - Exic calibration mode");
  Sprintln("ALL - Print all ratios limits");
  Sprintln("When in CALIB mode:");
  Sprintln("\t[value in cents] calibrates that coin type");
  Sprintln("\tRESET restarts max and min values for active coin type");
  printLimitsAll();
}

void serialWatchdog(){
  if (Serial.available()){
    String command = Serial.readString();
    if (command.equalsIgnoreCase("CALIB\n")){
      calibrate = 1;
      Sprintln("Entered calibration mode.\n");
    }
    else if (command.equalsIgnoreCase("RUN\n")) {
      calibrate = 0;
      Sprintln("Exited calibration mode.\n");
      printLimitsAll();
    }
    else if (command.equalsIgnoreCase("RESET\n")) {
      resetLimit(&coins[coin_id]);
      Sprint("Reset ratios limits of coin ID = ");
      Serial.print(coin_id);
      Sprintln(".\n");
    }
    else if (command.equalsIgnoreCase("ALL\n")) {
      printLimitsAll();
    }
    else {
      uint8_t calibrate_coin = command.toInt();
      Sprint("Will calibrate coin of ");
      Serial.print(calibrate_coin);
      Sprintln(" cents.\n");
      switch(calibrate_coin) {
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

void printLimitsAll(){
  Sprintln("Now coin ratios limits are:");
  Sprintln("Coin type:\t0(1c)\t1(2c)\t2(5c)\t3(10c)\t4(20c)\t5(50c)\t6(1e)\t7(2e)");
  Sprintln("\t\t-------------------------------------------------------------");
  Sprint("w3/d min:\t");
  for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w3_d); Sprint("\t");}
  Sprint("\nw3/d max:\t");
  for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w3_d); Sprint("\t");}
  Sprint("\n\n");
}

void printLimitsCoin(){
  Sprint("Now coin ID = ");
  Serial.print(coin_id);
  Sprintln(" ratios limits are:");
  Sprint("w3/d min:\t");
  Serial.print(coins[coin_id].min.w3_d);
  Sprint("\nw3/d max:\t");
  Serial.print(coins[coin_id].max.w3_d);
  Sprint("\n\n");
}

void resetLimit(coin_params* coin){
  coin->min.w3_d = 100;
  coin->max.w3_d = 0.0;
}

void newCoin(){    // Compares ratio with valid ranges
  uint8_t d = t3 - t2;
  uint8_t t = d + w3;
  coin_ratios ratios;
  new_coin = false;
  ratios.w3_d = float(w3) / d;
  ratios.w3_t = float(w3) / t;
  Sprint("New coin introduced:\t");
  #if DEBUG
    Serial.println(String(w2)+"\t"+String(w3)+"\t"+String(d)+"\t"+String(t));
  #endif
  Sprint("\nw3/d = ");
  Serial.println(ratios.w3_d);
  if (calibrate) {
    adjustCoin(ratios);
    if (Serial) printLimitsCoin();
  }
  else compareCoin(ratios);
}

void adjustCoin(coin_ratios ratios){
  if(coins[coin_id].min.w3_d > ratios.w3_d) coins[coin_id].min.w3_d = ratios.w3_d;
  if(coins[coin_id].max.w3_d < ratios.w3_d) coins[coin_id].max.w3_d = ratios.w3_d;
  if(coins[coin_id].min.w3_t < ratios.w3_t) coins[coin_id].min.w3_t = ratios.w3_t;
  if(coins[coin_id].max.w3_t < ratios.w3_t) coins[coin_id].max.w3_t = ratios.w3_t;
}

void compareCoin(coin_ratios ratios){
  uint8_t cents;
  if ((ratios.w3_d >= coins[0].min.w3_d) && (ratios.w3_d <= coins[0].max.w3_d)) cents = 1;     // Object detected as 1c coin
  else if ((ratios.w3_d >= coins[1].min.w3_d) && (ratios.w3_d <= coins[1].max.w3_d)) cents = 2;     // Object detected as 2c coin
  else if ((ratios.w3_d >= coins[2].min.w3_d) && (ratios.w3_d <= coins[2].max.w3_d)) cents = 5;     // Object detected as 5c coin
  else if ((ratios.w3_d >= coins[3].min.w3_d) && (ratios.w3_d <= coins[3].max.w3_d)) cents = 10;    // Object detected as 10c coin
  else if ((ratios.w3_d >= coins[4].min.w3_d) && (ratios.w3_d <= coins[4].max.w3_d)) cents = 20;    // Object detected as 20c coin
  else if ((ratios.w3_d >= coins[5].min.w3_d) && (ratios.w3_d <= coins[5].max.w3_d)) cents = 50;    // Object detected as 50c coin
  else if ((ratios.w3_d >= coins[6].min.w3_d) && (ratios.w3_d <= coins[6].max.w3_d)) cents = 100;   // Object detected as 100c coin
  else if ((ratios.w3_d >= coins[7].min.w3_d) && (ratios.w3_d <= coins[7].max.w3_d)) cents = 200;   // Object detected as 200c coin
  else {            // Object detected out of range
    cents = 0;
    Sprintln("Coin not detected in pre-calibrated ranges, try again.\n");
  }
  if (cents > 0) {
    Sprint("New coin detected of ");
    Serial.print(cents);
    Sprintln(" cents.");
    if ((cents > 5) && (cents < 200)){    // Accepted coins condition
      coinAccepted(cents);
    }
    else {
      Sprintln("Coin rejected, sorry.");
    }
  }
}

void coinAccepted(uint8_t cents){   // Acepts coin if ratio was validated
  payment_money += cents;     // Add coin value to money of payment in progress
  Sprint("Coin added to payment, total inserted money: ");
  Serial.print(payment_money);
  Sprintln(" cents.\n");
  if (payment_money >= 120){
    queue++;
    if (Serial){            // Debug people in queue
      Sprint("***** New payment completed: ");
      Serial.print(payment_money);
      Sprint(" cents, people in queue: ");
      Serial.print(queue);
      Sprintln(" *****");
    }
    payment_money = 0;
  }
  Sprint("\n");
  //openCollector();  
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
  pinMode(SO2, INPUT_PULLUP);
  pinMode(SO3, INPUT_PULLUP);
  pinMode(SW2, INPUT);
  pinMode(M1_en, OUTPUT);
  pinMode(M1_bk, OUTPUT);
  pinMode(L2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SO3), ISR_SO3, CHANGE);

  loadDefaultLimits();
  
  Serial.begin(9600);
  if (Serial) serialWelcome();
  
  digitalWrite(M1_bk, HIGH);

  calibrate = false;
  new_coin = false;
  coin_id = 8;
  payment_money = 0;
  queue = 0;
}

void loop() {  // ##################################################
  serialWatchdog();
  if (new_coin){
    newCoin();
  }
}
