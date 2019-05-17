#include "monedero.h"

#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"
#include <avr/interrupt.h>

#define SERIAL_DEBUG 
#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PIND,INT2)
#define writeM1_bk(value) wbi(PINL,6,value)
#define writeL2(value) wbi(PINL,1,value)

static void (*callback) ();

// Detection variables 
static unsigned long t2u, t3u, t2d, d, s;
static uint8_t payment_money;
static uint8_t coin_state;

static struct coin_params {
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

void monederoConfig() {  
	while (rbi(PINK,SW1));
	DDRL = 0xFF;
	
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

ISR(SO2_vect){   // ISR of first optic sensor
	if (readSO2()){
		if (coin_state == 0){
			t2u = micros();
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
			t2d = micros();
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
			t3u = micros();
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


void coinAccepted(uint8_t cents){   	// Accepts coin if ratio was validated
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
	//openCollector();
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





void monederoLoop() {
	if (coin_state == 4){
		coin_state = 0;
		newCoin();
	}
}

void monederoSetup() {
    monederoConfig();
    loadDefaultLimits();
    
    serialBegin(9600);
    
    writeM1_bk(true);

    coin_state = 0;
    payment_money = 0;
}

void monederoSetCallbackCorrecto(void(*f)()) {
	callback = f;
}

