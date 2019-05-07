// Main file for coin collector

// Precompiler definitions #########################################
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin assignment in "pindef.h
#include "default_coin_limits.h"  // Load pre-calibrated limits for coin detection
//#define DEBUG         // Uncomment for serial debugging

// Global variables ################################################

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
uint8_t time_SO2, time_SO3;
uint8_t width_SO2, width_SO3, delay_SO, total_SO;
float rel;
float rel_max = 0;
float rel_min = 0;

//Flags ------------------------------
uint8_t new_coin;
uint8_t calibrate, calibrate_coin;
uint8_t coin_id;

// Interrupt Service Routines ######################################

void ISR_SO2() {
  if (digitalRead(SO2)) {
    time_SO2 = millis();
  }
  else {
    width_SO2 = millis() - time_SO2;
    Serial.println("Moneda: ancho02 ancho03 dly total");
    Serial.print(String(width_SO2) + "   ");
  }
}

void ISR_SO3() {
  if (digitalRead(SO3)) {
    time_SO3 = millis();
    delay_SO = millis()- time_SO2;
  }
  else {
    width_SO3 = millis() - time_SO3;
    total_SO = millis() - time_SO2;
    Serial.print(String(width_SO3) + "   ");
    Serial.print(String(delay_SO) + "   ");
    Serial.print(String(total_SO) + "   ");
    Serial.println("Medida relativa: ");
    rel = float(width_SO3)/delay_SO;
    if(rel > rel_max) rel_max = rel;
    else if(rel < rel_min) rel_min = rel;
    Serial.print(String(rel) + "   " + "Max: " + String(rel_max) + "   " + "Min: " + String(rel_min));
  }
}
/*

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    time_SO2 = millis();
    new_coin = false;
    Serial.println("Entra SO2");
  }
  else {
    width_SO2 = millis() - time_SO2;
    Serial.println("Sale SO2");
  }
}

void ISR_SO3(){   // ISR of second optic sensor
  if (digitalRead(SO3)){
    time_SO3 = millis();
    delay_SO = time_SO3-time_SO2;
    Serial.println("Delay " + String(delay_SO));
  }
  else {
    width_SO3 = millis() - time_SO3;
    Serial.println("Width " + String(width_SO3));
    new_coin = true;
  }
}
*/
// Functions #######################################################

void serialWelcome(){
  Sprintln("Avaliable commands:");
  Sprintln("CALIBRATE - Calibrate ratios limits for specific coin type");
  Sprintln("RUN - Exic calibration mode");
  Sprintln("When in CALIB mode, [value in cents] calibrates that coin type");
  serialPrintLimits();
}

void serialWatchdog(){
  if (Serial.available()){
    String command = Serial.readString();
    Sprint("\nCommand received: ");
    Serial.println(command);
    if (command.equalsIgnoreCase("CALIBRATE")) calibrate = 1;
    else if (command.equalsIgnoreCase("RUN")) calibrate = 0;
    else {
      calibrate_coin = command.toInt();
      Sprint("Will calibrate coin of ");
      Serial.print(command);
      Sprintln(" cents.");
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

void serialPrintLimits(){
  Sprintln("Now coin ratios limits are:");
  Sprintln("\t\t1c\t2c\t5c\t10c\t20c\t50c\t1e\t2e");
  Sprint("w3/d min:\t");
  for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w3_d); Sprint("\t");}
 // Sprint("\nw3/d max:\t");
 // for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w3_d); Sprint("\t");}
  Sprint("\nw3/t min:\t");
  for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w3_t); Sprint("\t");}
 // Sprint("\nw3/t max:\t");
 // for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w3_t); Sprint("\t");}
}

void resetLimit(coin_params* coin){
  coin->min.w3_d = 100;
  coin->max.w3_d = 0.0;
//  coin->min.w3_t = 100;
//  coin->max.w3_t = 0.0;
}

void newCoin(){    // Compares ratio with valid ranges
//  delay_SO = uint8_t(time_SO3 - time_SO2);
  total_SO = delay_SO + width_SO3;
  coin_ratios ratios;
  new_coin = false;
  float w3_d = float(width_SO3) / delay_SO;
  ratios.w3_t = float(width_SO3) / total_SO;
  Sprint("New coin introduced, type ");
  Serial.println(coin_id);
  Sprint("widthO3/delay = ");
  Serial.println(w3_d);
 // Sprint("widthO3/total = ");
 // Serial.println(ratios.w3_t);
  if (calibrate) adjustCoin(ratios);
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
  if ((ratios.w3_d < coins[0].min.w3_d) || (ratios.w3_d > coins[7].max.w3_d)) {
    cents = 0;            // Object detected out of range
    Sprintln("Coin not detected in pre-calibrated ranges, try again");
  }
  else if ((ratios.w3_d >= coins[0].min.w3_d) && (ratios.w3_d <= coins[0].max.w3_d)) cents = 1;     // Object detected as 1c coin
  else if ((ratios.w3_d >= coins[1].min.w3_d) && (ratios.w3_d <= coins[1].max.w3_d)) cents = 2;     // Object detected as 2c coin
  else if ((ratios.w3_d >= coins[2].min.w3_d) && (ratios.w3_d <= coins[2].max.w3_d)) cents = 5;     // Object detected as 5c coin
  else if ((ratios.w3_d >= coins[3].min.w3_d) && (ratios.w3_d <= coins[3].max.w3_d)) cents = 10;    // Object detected as 10c coin
  else if ((ratios.w3_d >= coins[4].min.w3_d) && (ratios.w3_d <= coins[4].max.w3_d)) cents = 20;    // Object detected as 20c coin
  else if ((ratios.w3_d >= coins[5].min.w3_d) && (ratios.w3_d <= coins[5].max.w3_d)) cents = 50;    // Object detected as 50c coin
  else if ((ratios.w3_d >= coins[6].min.w3_d) && (ratios.w3_d <= coins[6].max.w3_d)) cents = 100;   // Object detected as 100c coin
  else if ((ratios.w3_d >= coins[7].min.w3_d) && (ratios.w3_d <= coins[7].max.w3_d)) cents = 200;   // Object detected as 200c coin
  else Sprintln("Unknown error");
  if ((cents > 5) && (cents < 200)){    // Accepted coins condition
    coinAccepted(cents);
  }
}

void coinAccepted(uint8_t cents){   // Acepts coin if ratio was validated
  payment_money += cents;     // Add coin value to money of payment in progress
  if (Serial){            // Debug money introduced
    Sprint("New coin accepted: ");
    Serial.print(cents);
    Sprintln(" cents, total inserted money: ");
    Serial.println(payment_money);
  }
  if (payment_money >= 120){
    queue++;
    if (Serial){            // Debug people in queue
      Sprint("New payment completed: ");
      Serial.print(payment_money);
      Sprint(" cents, people in queue: ");
      Serial.println(queue);
    }
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

  loadDefaultLimits();
  
  Serial.begin(9600);
 // if (Serial) serialWelcome();
  
  digitalWrite(M1_bk, HIGH);

  calibrate = false;
  new_coin = false;
  payment_money = 0;
  queue = 0;
}

void loop() {  // ##################################################
  //serialWatchdog();
  if (new_coin){
    newCoin();
  }
}
