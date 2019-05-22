#include "monedero.h"

#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"
#include <avr/interrupt.h>
#include <string.h>

#define SERIAL_DEBUG 
#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PINK,PCINT17)
#define writeM1_bk(value) wbi(PINL,6,value)
#define writeL2(value) wbi(PINL,1,value)

#define CALIB_TIMEOUT 10000
#define WALL_TIMEOUT 1000

static void (*callback) ();

// Detection variables 
unsigned long t, t2u, t3u, t2d, d, s, t_open;
static uint8_t payment_money;
uint8_t coin_state, calibrate, coin_id, wall;


struct coin_params {
	float ds_min;
	float ds_max;
};

static struct coin_params coins[8];


//Functions
void loadDefaultLimits(){       
	coins[0].ds_min = 10.0; coins[0].ds_max = 0.00;
	coins[1].ds_min = 4.38; coins[1].ds_max = 5.14;
	coins[2].ds_min = 2.57; coins[2].ds_max = 2.70;
	coins[3].ds_min = 3.28; coins[3].ds_max = 3.76;
	coins[4].ds_min = 2.25; coins[4].ds_max = 2.30;
	coins[5].ds_min = 1.75; coins[5].ds_max = 1.85;
	coins[6].ds_min = 1.92; coins[6].ds_max = 2.08;
	coins[7].ds_min = 1.55; coins[7].ds_max = 1.62;
}

// Interrupt Service Routines ######################################

uint16_t pinrut = PINK;
extern "C" void asm_ISR_SO2();
extern "C" void asm_ISR_SO3();

ISR(SO2_vect){   // ISR of first optic sensor
	t = micros();
	asm_ISR_SO2();
}

ISR(SO3_vect){   // ISR of first optic sensor
	t = micros();
	asm_ISR_SO3();
}

ISR(SW2_vect){
	writeM1_bk(1);
	if(!readSW2()){
		t_open = millis();
		wall = 0;
	}
	else {
		wall = 1;
	}
}

// CAL Functions ###################################################

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
	serialPrintLn("CAL - Calibrate ratios limits");
	serialPrintLn("RUN - Exit calibration mode");
	serialPrintLn("ALL - Print all ratios limits");
	serialPrintLn("When in CAL mode:");
	serialPrintLn("\t[value in cents] of coin");
	serialPrintLn("\tRES restarts coin limits");
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
    if (rbi(UCSR2A,RXC2)){
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
		serialPrintLn(": configuring 200c coin");
	} else serialPrintLn(": error, command not available");
    }
  }
}

// RUN Functions ###################################################

void coinAccepted(uint8_t cents){   	// Accepts coin if ratio was validated
	writeM1_bk(0);
	payment_money += cents;           	// Add coin value to money of payment in progress
	serialPrint("Coin added to payment, total inserted money: ");
	serialPrintInt(payment_money);
	serialPrintLn(" cents.\n");
	if (payment_money >= 120){
		callback();
		serialPrint("***** New payment completed: ");
		serialPrintInt(payment_money);
		serialPrintLn(" *****");
		payment_money = 0;
	}
	serialPrint("\n");
	
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

// Core Functions ###################################################

void newCoin(){
	float ds;
	d = t3u - t2u;
	s = t2d - t3u;
	ds = (float)d / s;
	serialPrint("New coin introduced:\t");
	#ifdef SERIAL_DEBUG
	serialPrint("td\ts");
	serialPrintInt(d); 	serialPrint("\t");
	serialPrintInt(s);
	#endif
	serialPrint("\nwd/s = ");
	serialPrintFloat(ds);
	compareCoin(ds);
}

void monederoConfig() {   // Configure I/O ports and interruptions for monedero block
	while (rbi(PINK,SW1));
	DDRL = 0xFF;		// Configure output port
	cli();
	sbi(EICRA,ISC10);   // INT1 trigger
	cbi(EICRA,ISC11);
	sbi(EICRA,ISC20);	// INT2 trigger
	cbi(EICRA,ISC21);
	sbi(EIMSK,INT1);	// Interrupt mask enable
	sbi(EIMSK,INT2);
	PCMSK2 = 0x02;
	sbi(PCICR, PCIE2);
	sei();
}

void initWall(){
	if(!readSW2()) writeM1_bk(0);
	}

void monederoSetup() {
    monederoConfig();
    loadDefaultLimits();
	initTime();
	initWall();
    serialBegin(9600);
    serialWelcome();
    writeM1_bk(true);
	calibrate = false;
    coin_state = 0;
	coin_id = 8;
    payment_money = 0;
	while((CALIB_TIMEOUT - millis()) > 0) serialWatchdog();
	while(calibrate == true) serialWatchdog();
}


void monederoLoop() {
	if (coin_state == 4){
		coin_state = 0;
		newCoin();
	}
	if(wall == 0){ 
		if(millis() - t_open > WALL_TIMEOUT){
			writeM1_bk(0);
		}
	}
}

void monederoSetCallbackCorrecto(void(*f)()) {
	callback = f;
}

void monederoParar() {

}