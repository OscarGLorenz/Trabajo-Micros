// Main file for coin collector

// Precompiler definitions #########################################

#include "monedero.h"
#include "../common/macros.h"
#include "../common/time.h"
#include "../common/pinout.h"
#include "../common/serial.h"
#include "monederoVars.h"     // Load pre-calibrated limits for coin detection

#define SERIAL_DEBUG         // Uncomment for serial debugging

#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PIND,INT2)
#define writeM1_en(value) wbi(PINL,4,value)
#define writeM1_bk(value) wbi(PINL,6,value)
#define writeL2(value) wbi(PINL,1,value)

// Interrupt Service Routines ######################################

ISR(SO2_vect){   // ISR of first optic sensor
	if (readSO2()){
		if (coin_state == 0){
			t2 = micros();
			coin_state = 1;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Entra SO2"); 
			#endif
		} else {
			coin_state = 0;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Espurio SO2 - rise"); 
			#endif
		}
	} else {
		if (coin_state == 2){
			w2 = micros() - t2;
			coin_state = 3;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Sale SO2"); 
			#endif
		} else {
			coin_state = 0;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Espurio SO2 - fall"); 
			#endif
		}
	}
}

ISR(SO3_vect){   // ISR of first optic sensor
	if (readSO3()){
		if (coin_state == 1){
			t3 = micros();
			coin_state = 2;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Entra SO3"); 
			#endif
		} else {
			coin_state = 0;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Espurio SO3 - rise"); 
			#endif
		}
	} else {
		if (coin_state == 3){
			w3 = micros() - t3;
			coin_state = 4;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Sale SO3"); 
			#endif
		} else {
			coin_state = 0;
			#ifdef SERIAL_DEBUG
				serialPrintLn("Espurio SO3 - fall"); 
			#endif
		}
	}
}

// Functions #######################################################

void printLimitsCoin(){
	serialPrint("Now coin ID = ");
	serialPrintInt(coin_id);
	serialPrintLn(" ratios limits are:");
	serialPrint("d/s min:\t"); serialPrintFloat(coins[coin_id].ds_min); serialPrint("\n");
	serialPrint("d/s max:\t"); serialPrintFloat(coins[coin_id].ds_max); serialPrint("\n");
	serialPrint("\n");
}

void printLimitsAll(){
	serialPrintLn("Now coin ratios limits are:");
	serialPrintLn("Coin type:\t0(1c)\t1(2c)\t2(5c)\t3(10c)\t4(20c)\t5(50c)\t6(1e)\t7(2e)");
	serialPrintLn("\t\t-------------------------------------------------------------");
	serialPrint("d/s min:\t"); 
	for(int i = 0; i<8 ; i++) {
		serialPrintFloat(coins[i].ds_min);
		serialPrint("\t");
	}
	serialPrint("\nd/s max:\t");
	for(int i = 0; i<8 ; i++) {
		serialPrintFloat(coins[i].ds_max); 
		serialPrint("\t");
	}
	serialPrintLn("\n");
}

void resetLimit(struct coin_params* coin){
	coin->ds_min = 100;
	coin->ds_max = 0.0;
}

void serialWelcome(){
	serialPrintLn("Available commands:");
	serialPrintLn("CALIB - Calibrate ratios limits for specific coin type");
	serialPrintLn("RUN - Exic calibration mode");
	serialPrintLn("ALL - Print all ratios limits");
	serialPrintLn("When in CALIB mode:");
	serialPrintLn("\t[value in cents] calibrates that coin type");
	serialPrintLn("\tRESET restarts max and min values for active coin type");
	printLimitsAll();
}

void serialWatchdog(){
  static uint8_t delay = 0;
  static char command[20]="";
  if(delay < 255) {
    delay++;
  }
  else {
    delay = 0;
    if (rbi(UCSR0A,RXC0)){
    serialReadString(command);
    serialPrint(command);
    if(!strcmp(command,"cal")){
		serialPrintLn(": CALIBRATE MODE, SELECT COIN!");
		calibrate = 1;
    } else if (!strcmp(command,"run")){
		serialPrintLn(": RUN MODE");
		calibrate = 0;
		printLimitsAll();
	} else if (!strcmp(command,"res")){
		serialPrintLn(": RESET LIMIT OF SELECTED COIN");
		resetLimit(&coins[coin_id]);
	} else if (!strcmp(command,"all")){
		serialPrintLn(": SHOW CALIBRATED RANGES");
		printLimitsAll();
    } else if (!strcmp(command,"2")){
		coin_id = 1;
		serialPrintLn(": configuring 2c coin");
	} else if (!strcmp(command,"5")){
		coin_id = 2;
		serialPrintLn(": configuring 5c coin");
    } else if (!strcmp(command,"10")){
		coin_id = 3;
		serialPrintLn(": configuring 10c coin");
	} else if (!strcmp(command,"20")){
		coin_id = 4;
		serialPrintLn(": configuring 20c coin");
	} else if (!strcmp(command,"50")){
		coin_id = 5;
		serialPrintLn(": configuring 50c coin");
	} else if (!strcmp(command,"100")){
		coin_id = 6;
		serialPrintLn(": configuring 100c coin");
	} else if (!strcmp(command,"200")){
		coin_id = 7;
		serialPrintLn(": configuring 500c coin");
    else serialPrintLn(": error, command not available");
    }
  }
}
 /*
void serialWatchdog(){
  if (Serial.available()){
    String command = Serial.readString();
    if (command.equalsIgnoreCase("CALIB\n")){
      calibrate = 1;
      serialPrintLn("Entered calibration mode.\n");
    }
    else if (command.equalsIgnoreCase("RUN\n")) {
      calibrate = 0;
      serialPrintLn("Exited calibration mode.\n");
      printLimitsAll();
    }
    else if (command.equalsIgnoreCase("RESET\n")) {
      resetLimit(&coins[coin_id]);
      serialPrint("Reset ratios limits of coin ID = ");
      Serial.print(coin_id);
      serialPrintLn(".\n");
    }
    else if (command.equalsIgnoreCase("ALL\n")) {
      printLimitsAll();
    }
    else {
      uint8_t calibrate_coin = command.toInt();
      serialPrint("Will calibrate coin of ");
      Serial.print(calibrate_coin);
      serialPrintLn(" cents.\n");
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
*/

void wait(int milliseconds){
	unsigned long int prev_time = millis();
	while ((millis() - prev_time) < milliseconds) {}
}

void openCollector(){
	writeM1_en(true);    	// Open coin slot
	while (readSW2()) {}
	wait(10);              // Substitute with timer interrupt!!
	writeM1_en(false);     // Wait for coin to pass thorugh
	wait(100);
	writeM1_en(true);      // Close coin slot
	while (!readSW2()) {}
	wait(10);
	writeM1_en(false);     // Keep in rejection position
}

void coinAccepted(uint8_t cents){   	// Acepts coin if ratio was validated
	payment_money += cents;           	// Add coin value to money of payment in progress
	serialPrint("Coin added to payment, total inserted money: ");
	serialPrintInt(payment_money);
	serialPrintLn(" cents.\n");
	if (payment_money >= 120){
		queue++;
		serialPrint("***** New payment completed: ");
		serialPrintInt(payment_money);
		serialPrint(" cents, people in queue: ");
		serialPrintInt(queue);
		serialPrintLn(" *****");
		payment_money = 0;
	}
	serialPrint("\n");
	//openCollector();  
}

void adjustCoin(float ds){
	if(coins[coin_id].ds_min > ds) {
		coins[coin_id].ds_min = ds;
	} else if(coins[coin_id].ds_max < ds) {
		coins[coin_id].ds_max = ds;
	}
}

void compareCoin(float ds){
	uint8_t cents;
		 if ((ds >= coins[1].ds_min) && (ds <= coins[1].ds_max)) cents = 2;     // Object detected as 2c coin
	else if ((ds >= coins[2].ds_min) && (ds <= coins[2].ds_max)) cents = 5;     // Object detected as 5c coin
	else if ((ds >= coins[3].ds_min) && (ds <= coins[3].ds_max)) cents = 10;    // Object detected as 10c coin
	else if ((ds >= coins[4].ds_min) && (ds <= coins[4].ds_max)) cents = 20;    // Object detected as 20c coin
	else if ((ds >= coins[5].ds_min) && (ds <= coins[5].ds_max)) cents = 50;    // Object detected as 50c coin
	else if ((ds >= coins[6].ds_min) && (ds <= coins[6].ds_max)) cents = 100;   // Object detected as 100c coin
	else if ((ds >= coins[7].ds_min) && (ds <= coins[7].ds_max)) cents = 200;   // Object detected as 200c coin
	//else if ((ds > coins[2].ds_max) || (ds < coins[7].ds_min)) cents = 1;   // Object detected as 1c coin
	else {            // Object detected out of range
		cents = 0;
		serialPrintLn("Coin not detected in pre-calibrated ranges, try again.\n");
	}
	if (cents > 0) {
		serialPrint("New coin detected of ");
		serialPrintInt(cents);
		serialPrintLn(" cents.");
		if ((cents > 5) && (cents < 200)){    // Accepted coins condition
			coinAccepted(cents);
		} else {
			serialPrintLn("Coin rejected, sorry.");
		}
	}
}

void newCoin(){
	float ds;
	d = t3 - t2;
	s = w2 - (t3 - t2);
	ds = (float)d / s;
	serialPrint("New coin introduced:\t");
	#ifdef SERIAL_DEBUG
		serialPrint("w2\tw3\td\ts");
		serialPrintInt(w2); serialPrint("\t");
		serialPrintInt(w3); serialPrint("\t");
		serialPrintInt(d); 	serialPrint("\t");
		serialPrintInt(s);
	#endif
	serialPrint("\nwd/s = ");
	serialPrintFloat(ds);
	if (calibrate) {
		adjustCoin(ds);
		printLimitsCoin();
	} else {
		compareCoin(ds);
	}
}

void monederoConfig() {   // Configure I/O ports and interruptions for monedero block
	while (rbi(PINK,SW1));
	DDRL = 0xFF;		// Configure output port
	
	cli();
	
	// Interrupt triggers
	sbi(EICRA,ISC10);
	cbi(EICRA,ISC11);
	sbi(EICRA,ISC20);
	cbi(EICRA,ISC21);
	
	//Interrupt enable
	sbi(EIMSK,INT1);
	sbi(EIMSK,INT2);
	
	sei();
}

void monederoSetup() {  // #################################################
  
  monederoConfig();
  loadDefaultLimits();
  
  serialBegin(9600);
  serialWelcome();
  
  writeM1_bk(true);

  calibrate = false;
  coin_state = 0;
  coin_id = 8;
  payment_money = 0;
  queue = 0;
}

void monederoLoop() {  // ##################################################
// 	serialWatchdog();
	if (coin_state == 4){
		coin_state = 0;
		newCoin();
	}
}
