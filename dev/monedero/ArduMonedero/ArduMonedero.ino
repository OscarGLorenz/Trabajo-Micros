// Main file for coin collector

// Precompiler definitions #########################################
//#include "time.h"
#include <Arduino.h>  // For Serial standard functions
#include "macros.h"   // For macros, some of them ported from Marlin
#include "pindef.h"   // Check pin assignment in "pindef.h
//#include "serial.h"

#include "default_coin_limits.h"  // Load pre-calibrated limits for coin detection
#define DEBUG         // Uncomment for serial debugging

// Global variables ################################################

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
unsigned long t2, t3, w2, w3, d, t, s;

//Flags ------------------------------
uint8_t coin_state;
uint8_t calibrate;
uint8_t coin_id;

// Interrupt Service Routines ######################################

void ISR_SO2(){   // ISR of first optic sensor
  if (digitalRead(SO2)){
    if (coin_state == 0){
      t2 = micros();
      coin_state = 1;
      #ifdef DEBUG
        Sprintln("Entra SO2"); 
      #endif
    }
    else {
      coin_state = 0;
      #ifdef DEBUG
      Sprintln("Espurio SO2 - rise"); 
    #endif
    }
  }
  else if (coin_state == 2){
    w2 = micros() - t2;
    coin_state = 3;
    #ifdef DEBUG
      Sprintln("Sale SO2"); 
    #endif
  }
  else {
    coin_state = 0;
    #ifdef DEBUG
      Sprintln("Espurio SO2 - fall"); 
    #endif
  }
}

void ISR_SO3(){   // ISR of second optic sensor
  if (digitalRead(SO3)){
    if (coin_state == 1){
      t3 = micros();
      coin_state = 2;
      #ifdef DEBUG
        Sprintln("Entra SO3"); 
      #endif
    }
    else{
      coin_state = 0;
      #ifdef DEBUG
        Sprintln("Espurio SO3 - rise"); 
      #endif
    }
  }
  else if (coin_state == 3){
    w3 = micros() - t3;
    coin_state = 4;
    #ifdef DEBUG
      Sprintln("Sale SO3");
    #endif
  }
  else {
    coin_state = 0;
    #ifdef DEBUG
      Sprintln("Espurio SO3 - fall"); 
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
  Sprint("w2/d min:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w2_d); Sprint("\t");} Sprint("\n");
  Sprint("w2/d max:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w2_d); Sprint("\t");} Sprint("\n\n");
  Sprint("w3/d min:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w3_d); Sprint("\t");} Sprint("\n");
  Sprint("w3/d max:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w3_d); Sprint("\t");} Sprint("\n\n");
  Sprint("w2/t min:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w2_t); Sprint("\t");} Sprint("\n");
  Sprint("w2/t max:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w2_t); Sprint("\t");} Sprint("\n\n");
  Sprint("w3/t min:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.w3_t); Sprint("\t");} Sprint("\n");
  Sprint("w3/t max:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.w3_t); Sprint("\t");} Sprint("\n\n");
  Sprint("d/s min:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].min.d_s); Sprint("\t");} Sprint("\n");
  Sprint("d/s max:\t"); for(int i = 0; i<8 ; i++){ Serial.print(coins[i].max.d_s); Sprint("\t");} Sprint("\n");
  Sprint("\n");
}

void printLimitsCoin(){
  Sprint("Now coin ID = ");
  Serial.print(coin_id);
  Sprintln(" ratios limits are:");
  Sprint("w2/d min:\t"); Serial.print(coins[coin_id].min.w2_d); Sprint("\n");
  Sprint("w2/d max:\t"); Serial.print(coins[coin_id].max.w2_d); Sprint("\n");
  Sprint("w3/d min:\t"); Serial.print(coins[coin_id].min.w3_d); Sprint("\n");
  Sprint("w3/d max:\t"); Serial.print(coins[coin_id].max.w3_d); Sprint("\n");
  Sprint("w2/t min:\t"); Serial.print(coins[coin_id].min.w2_t); Sprint("\n");
  Sprint("w2/t max:\t"); Serial.print(coins[coin_id].max.w2_t); Sprint("\n");
  Sprint("w3/t min:\t"); Serial.print(coins[coin_id].min.w3_t); Sprint("\n");
  Sprint("w3/t max:\t"); Serial.print(coins[coin_id].max.w3_t); Sprint("\n");
  Sprint("d/s min:\t"); Serial.print(coins[coin_id].min.d_s); Sprint("\n");
  Sprint("d/s max:\t"); Serial.print(coins[coin_id].max.d_s); Sprint("\n");
  Sprint("\n");
}

void resetLimit(coin_params* coin){
  coin->min.w2_d = 100;
  coin->max.w2_d = 0.0;
  coin->min.w3_d = 100;
  coin->max.w3_d = 0.0;
  coin->min.w2_t = 100;
  coin->max.w2_t = 0.0;
  coin->min.w3_t = 100;
  coin->max.w3_t = 0.0;
  coin->min.d_s = 100;
  coin->max.d_s = 0.0;
}

void newCoin(){    // Compares ratio with valid ranges
  d = t3 - t2;
  t = d + w3;
  s = w2 - (t3 - t2);
  struct coin_ratios ratios;
  ratios.w2_d = float(w2) / d;
  ratios.w3_d = float(w3) / d;
  ratios.w2_t = float(w2) / t;
  ratios.w3_t = float(w3) / t;
  ratios.d_s = float(d) / s;
  Sprint("New coin introduced:\t");
  #ifdef DEBUG
    Sprintln("w2\tw3\td\tt\ts");
    Serial.println("\t\t\t"+String(w2)+"\t"+String(w3)+"\t"+String(d)+"\t"+String(t)+"\t"+String(s));
  #endif
  Sprint("\n");
  Sprint("w2/d = "); Serial.print(ratios.w2_d); Sprint(",\t");
  Sprint("w3/d = "); Serial.print(ratios.w3_d); Sprint(",\t");
  Sprint("w2/t = "); Serial.print(ratios.w2_t); Sprint(",\t");
  Sprint("w3/t = "); Serial.print(ratios.w3_t); Sprint(",\t");
  Sprint("d/s = "); Serial.print(ratios.d_s); Sprint("\n");
  if (calibrate) {
    adjustCoin(ratios);
    if (Serial) printLimitsCoin();
  }
  else compareCoin(ratios);
}

void adjustCoin(struct coin_ratios ratios){
  if(coins[coin_id].min.w2_d > ratios.w2_d) coins[coin_id].min.w2_d = ratios.w2_d;
  if(coins[coin_id].max.w2_d < ratios.w2_d) coins[coin_id].max.w2_d = ratios.w2_d;
  if(coins[coin_id].min.w3_d > ratios.w3_d) coins[coin_id].min.w3_d = ratios.w3_d;
  if(coins[coin_id].max.w3_d < ratios.w3_d) coins[coin_id].max.w3_d = ratios.w3_d;
  if(coins[coin_id].min.w2_t > ratios.w2_t) coins[coin_id].min.w2_t = ratios.w2_t;
  if(coins[coin_id].max.w2_t < ratios.w2_t) coins[coin_id].max.w2_t = ratios.w2_t;
  if(coins[coin_id].min.w3_t > ratios.w3_t) coins[coin_id].min.w3_t = ratios.w3_t;
  if(coins[coin_id].max.w3_t < ratios.w3_t) coins[coin_id].max.w3_t = ratios.w3_t;
  if(coins[coin_id].min.d_s > ratios.d_s) coins[coin_id].min.d_s = ratios.d_s;
  if(coins[coin_id].max.d_s < ratios.d_s) coins[coin_id].max.d_s = ratios.d_s;
}

void compareCoin(struct coin_ratios ratios){   // Cambiar lo que se compara en casos en los que se solapen algunos ratios
  uint8_t cents;
  #ifdef DEBUG
    Serial.println(ratios.d_s);
  #endif
  if ((ratios.d_s >= coins[1].min.d_s) && (ratios.w3_d <= coins[1].max.d_s)) cents = 2;     // Object detected as 2c coin
  else if ((ratios.d_s >= coins[2].min.d_s) && (ratios.d_s <= coins[2].max.d_s)) cents = 5;     // Object detected as 5c coin
  else if ((ratios.d_s >= coins[3].min.d_s) && (ratios.d_s <= coins[3].max.d_s)) cents = 10;    // Object detected as 10c coin
  else if ((ratios.d_s >= coins[4].min.d_s) && (ratios.d_s <= coins[4].max.d_s)) cents = 20;    // Object detected as 20c coin
  else if ((ratios.d_s >= coins[5].min.d_s) && (ratios.d_s <= coins[5].max.d_s)) cents = 50;    // Object detected as 50c coin
  else if ((ratios.d_s >= coins[6].min.d_s) && (ratios.d_s <= coins[6].max.d_s)) cents = 100;   // Object detected as 100c coin
  else if ((ratios.d_s >= coins[7].min.d_s) && (ratios.d_s <= coins[7].max.d_s)) cents = 200;   // Object detected as 200c coin
  else if ((ratios.d_s > coins[2].max.d_s) && (ratios.d_s < coins[7].min.d_s)) cents = 1;     // Object detected as 1c coin
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
  openCollector();
}

void wait(int milliseconds){
  unsigned long int prev_time = millis();
  while ((millis() - prev_time) < milliseconds){
  }
}

void openCollector(){
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
  pinMode(8, OUTPUT);
  pinMode(M1_bk, OUTPUT);
  pinMode(L2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(SO2), ISR_SO2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SO3), ISR_SO3, CHANGE);

  loadDefaultLimits();
  
  Serial.begin(9600);
  if (Serial) serialWelcome();
  
  digitalWrite(M1_bk, LOW);

  calibrate = false;
  coin_id = 8;
  coin_state = 0;
  payment_money = 0;
  queue = 0;
}

void loop() {  // ##################################################
  serialWatchdog();
  if (coin_state == 4){
    coin_state = 0;
    newCoin();
  }
}
