#include <string.h>
//#include <stdbool.h>

#include "monedero.h"

#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"
//#define SERIAL_DEBUG

#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PINK,PCINT17)


#define CALIB_TIMEOUT 10000
#define WALL_TIMEOUT 2

static void (*callback) ();

// Detection variables
unsigned long t_coin, t2u, t3u, t2d ;
static unsigned long d, s, t_close, t_led;

// Flags
static uint8_t led, calibrate, lobotomy;
uint8_t coin_state, pin_sensor;
static uint8_t payment_money, coin_id;
static uint8_t wall;

struct coin_params {
    float ds_min;
    float ds_max;
};

static struct coin_params coins[8];

void loadDefaultLimits(){
    coins[0].ds_min = 10.0; coins[0].ds_max = 0.00;		// 1c coin
    coins[1].ds_min = 4.38; coins[1].ds_max = 5.14;		// 2c coin
    coins[2].ds_min = 2.57; coins[2].ds_max = 2.70;		// 5c coin
    coins[3].ds_min = 3.28; coins[3].ds_max = 3.76;		// 10c coin
    coins[4].ds_min = 2.25; coins[4].ds_max = 2.30;		// 20c coin
    coins[5].ds_min = 1.75; coins[5].ds_max = 1.85;		// 50c coin
    coins[6].ds_min = 1.90; coins[6].ds_max = 2.20;		// 100c coin
    coins[7].ds_min = 1.55; coins[7].ds_max = 1.62;		// 200c coin
}

void newCoin();
/*
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
                coin_state = 0;
				newCoin();
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
*/

extern void asm_ISR_SO2();
extern void asm_ISR_SO3();

ISR(SO2_vect){   // ISR of first optic sensor
	pin_sensor = PIND;
	t_coin = micros();
	asm_ISR_SO2();
	if(coin_state == 3) newCoin();
}

ISR(SO3_vect){   // ISR of first optic sensor
	pin_sensor = PIND;
	t_coin = micros();
	asm_ISR_SO3();
	
}

ISR(SW2_vect){
	sbi(OUTRUT,M1_bk);
	if(readSW2()&&(!lobotomy)){
		t_close = millis();
		wall = 1;
	} else if (lobotomy){
		cbi(EIMSK,INT1);	// Interrupt mask enable
		cbi(EIMSK,INT2);
		cbi(PCICR, PCIE2);
	} else {
		wall = 0;
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
  char command[20]="";
  static uint8_t delay = 0;
  if(delay < 255) {
    delay++;
  }
  else {
    delay = 0;
    if (rbi(UCSR0A,RXC0)){
    serialReadString(command);
    serialPrint(command);
    if(!strcmp(command,"cal"))serialPrintLn(": CALIBRATE MODE");
    else if (!strcmp(command,"run"))serialPrintLn(": RUN MODE");
    else if (!strcmp(command,"2"))serialPrintLn(": configuring 2c coin");
    else if (!strcmp(command,"5"))serialPrintLn(": configuring 5c coin");
    else if (!strcmp(command,"10"))serialPrintLn(": configuring 10c coin");
    else if (!strcmp(command,"20"))serialPrintLn(": configuring 20c coin");
    else if (!strcmp(command,"50"))serialPrintLn(": configuring 50c coin");
    else if (!strcmp(command,"100"))serialPrintLn(": configuring 100c coin");
    else if (!strcmp(command,"100"))serialPrintLn(": configuring 200c coin");
    else serialPrintLn(": error, command not available");
    }
  }
}


// RUN Functions ###################################################

void coinAccepted(uint8_t cents){   	// Accepts coin if ratio was validated
    payment_money += cents;           	// Add coin value to money of payment in progress
    serialPrint("\nCoin of ");
	serialPrintInt(cents);
	serialPrintLn(" added to payment, total inserted money: ");
    serialPrintInt(payment_money);
    serialPrintLn(" cents.\n");
    if (payment_money >= 120){
        callback();
        sbi(OUTRUT,L2);
        t_led = millis();
        led = 1;
        serialPrint("***** New payment completed: ");
        serialPrintInt(payment_money);
        serialPrintLn(" *****");
        payment_money = 0;
    }
    serialPrint("\n");

}

void compareCoin(float ds){
    uint8_t cents;
	if ((ds >= coins[3].ds_max) || (ds <= coins[5].ds_min)) {
		cbi(OUTRUT,M1_bk);					// Motor switched on to open wall
	}
	if ((ds >= coins[2].ds_min) && (ds <= coins[2].ds_max)) {cents = 5; cbi(OUTRUT,M1_bk);}     // Object detected as 5c coin
	else if ((ds >= coins[1].ds_min) && (ds <= coins[1].ds_max)) cents = 2;     // Object detected as 2c coin
	else if ((ds >= coins[7].ds_min) && (ds <= coins[7].ds_max)) cents = 200;   // Object detected as 200c coin
	else if ((ds >= coins[6].ds_min) && (ds <= coins[6].ds_max)) cents = 100;   // Object detected as 100c coin
	else if ((ds >= coins[5].ds_min) && (ds <= coins[5].ds_max)) cents = 50;    // Object detected as 50c coin
	else if ((ds >= coins[4].ds_min) && (ds <= coins[4].ds_max)) cents = 20;    // Object detected as 20c coin
	else if ((ds >= coins[3].ds_min) && (ds <= coins[3].ds_max)) cents = 10;    // Object detected as 10c coin
    else cents = 1;
	
    if ((cents < 10) || (cents > 100)){    // Accepted coins condition
        serialPrintLn("\nCoin rejected, sorry.");
		cbi(OUTRUT,M1_bk);					// Motor switched on to open wall
    } else {
		coinAccepted(cents);
    }
	#ifdef SERIAL_DEBUG
		serialPrint("\nwd/s =");
		serialPrintFloat(ds);
		serialPrint("\n");
	#endif
}

// Core Functions ###################################################

void monederoParar(){
	serialPrintLn("\nMonedero out of service :(");
	cbi(OUTRUT,M1_bk);
	lobotomy = 1;
}

void newCoin(){
    float ds;
    d = t3u - t2u;
    s = t2d - t3u;
    ds = (float)d / s;
#ifdef SERIAL_DEBUG
    serialPrint("New coin introduced:\t");
    serialPrint("d\ts\n\t\t\t");
    serialPrintInt(d); 	serialPrint("\t");
    serialPrintInt(s);
	#endif
    compareCoin(ds);
}

void monederoConfig() {   // Configure I/O ports and interruptions for monedero block
    cli();
    sbi(EICRA,ISC10);   // INT1 trigger
    cbi(EICRA,ISC11);
    sbi(EICRA,ISC20);	// INT2 trigger
    cbi(EICRA,ISC21);
    sbi(EIMSK,INT1);	// Interrupt mask enable
    sbi(EIMSK,INT2);
    sbi(PCMSK2,PCINT17);
    sbi(PCICR, PCIE2);
	cbi(DDRD, 1);
	cbi(DDRD, 2);
	sbi(DDRL, 6);
    sei();
}

void initWall(){
	
	if(readSW2()){
		#ifdef SERIAL_DEBUG
			serialPrintLn("Still closed");
		#endif
		cbi(OUTRUT,M1_bk);
	}
}

void monederoSetup() {

       monederoConfig();
       loadDefaultLimits();
       initWall();
       serialWelcome();
       cbi(OUTRUT,L2);
       calibrate = false;
       coin_state = 0;
       coin_id = 8;
       led = 0;
       payment_money = 0;
	   wall = 0;
       //while((CALIB_TIMEOUT - millis()) > 0) serialWatchdog();
       //while(calibrate == true) serialWatchdog();
}

#define LOBOTIME 20000

void monederoLoop() {
	if(wall){
        if(millis() - t_close > WALL_TIMEOUT){
            cbi(OUTRUT,M1_bk);
        }
    }
    if(led){
        if(millis() - t_led > 1000){
            cbi(OUTRUT,L2);
            led = 0;
        }
    }
	
	if ((millis()>LOBOTIME)&&!lobotomy) {
		monederoParar();
	}
}

void monederoSetCallbackCorrecto(void(*f)()) {
    callback = f;
}
