#include "monedero.h"

#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"
#include <string.h>
//#define SERIAL_DEBUG

#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PINK,PCINT17)


#define CALIB_TIMEOUT 10000
#define WALL_TIMEOUT_OPEN 1
#define WALL_TIMEOUT_WAIT 200
#define WALL_TIMEOUT_CLOSE 1

static void (*callback) ();

// Detection variables
static unsigned long t, t2u, t3u, t2d, d, s, t_wall, t_led;

// Flags
static uint8_t open_wall, close_wall, wall_moving;
static uint8_t led, calibrate, coin_state;
static uint8_t payment_money, coin_id;
static uint8_t wall_state;     // 0: open; 1: closing; 2: waiting; 3: opening;

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
    coins[6].ds_min = 1.9; coins[6].ds_max = 2.2;		// 100c coin
    coins[7].ds_min = 1.55; coins[7].ds_max = 1.62;		// 200c coin
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

void newCoin();

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

ISR(SW2_vect){
	if((open_wall || close_wall) && !wall_moving){
		t_wall = millis();
		wall_moving = 1;
		serialPrintLn("SW2 Change");
	}
/*	if(open_wall && !readSW2()){
		open_wall = 0;
		close_wall = 1;
	} else if(close_wall && readSW2()){
		close_wall = 0;
		|| close_wall){
       // sbi(OUTRUT,M1_bk);
      //  if(!readSW2()){
            
            open_wall = 0;
			close_wall = 1;
			serialPrintLn("Abierta");
        }   
	}
	open_wall = 0;
	*/
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
	cbi(OUTRUT,M1_bk);					// Motor switched on to open wall
	wall_state = 1;
    payment_money += cents;           	// Add coin value to money of payment in progress
    serialPrint("Coin of ");
	serialPrintInt(cents);
	serialPrintLn(" added to payment, total inserted money: ");
    serialPrintInt(payment_money);
    serialPrintLn(" cents.\n");
    if (payment_money >= 120){
        //callback();
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
	if ((ds >= coins[6].ds_min) && (ds <= coins[6].ds_max)) cents = 100;   // Object detected as 100c coin
	else if ((ds >= coins[5].ds_min) && (ds <= coins[5].ds_max)) cents = 50;    // Object detected as 50c coin
	else if ((ds >= coins[4].ds_min) && (ds <= coins[4].ds_max)) cents = 20;    // Object detected as 20c coin
	else if ((ds >= coins[3].ds_min) && (ds <= coins[3].ds_max)) cents = 10;    // Object detected as 10c coin
    else if ((ds >= coins[1].ds_min) && (ds <= coins[1].ds_max)) cents = 2;     // Object detected as 2c coin
    else if ((ds >= coins[2].ds_min) && (ds <= coins[2].ds_max)) cents = 5;     // Object detected as 5c coin
	else if ((ds >= coins[7].ds_min) && (ds <= coins[7].ds_max)) cents = 200;   // Object detected as 200c coin
    //else if ((ds > coins[2].ds_max) || (ds < coins[7].ds_min)) cents = 1;   // Object detected as 1c coin
    else {            // Object detected out of range
        cents = 0;
        serialPrintLn("Coin not detected in pre-calibrated ranges, try again.\n");
    }
    if ((cents > 5) && (cents < 200)){    // Accepted coins condition
        coinAccepted(cents);
    } else {
        serialPrintLn("Coin rejected, sorry.");
    }
}

// Core Functions ###################################################

void newCoin(){
    float ds;
    d = t3u - t2u;
    s = t2d - t3u;
    ds = (float)d / s;
#ifdef SERIAL_DEBUG
    serialPrint("New coin introduced:\t");
    serialPrint("td\ts");
    serialPrintInt(d); 	serialPrint("\t");
    serialPrintInt(s);
    serialPrint("\nwd/s =");
    serialPrintFloat(ds);
	serialPrint("\n");
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
    sei();
}

void initWall(){
	cbi(OUTRUT,M1_bk);
	while(readSW2()){
		serialPrintLn("Still closed");
	}
	sbi(OUTRUT,M1_bk);
	wall_state = 0;
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
	   open_wall = 0;
	   //wall = 1;
       //while((CALIB_TIMEOUT - millis()) > 0) serialWatchdog();
       //while(calibrate == true) serialWatchdog();
}

void checkWall(){
	if((wall_state == 1) && wall_moving){
		if(millis() - t_wall > WALL_TIMEOUT_CLOSE){
			sbi(OUTRUT,M1_bk);
			wall_moving = 0;
			t_wall = millis();
			wall_state = 2;
			serialPrintLn("Fully closed");
		}
	} else if((wall_state == 2)){
		if(millis() - t_wall > WALL_TIMEOUT_WAIT){
			cbi(OUTRUT,M1_bk);
			wall_state = 3;
			serialPrintLn("Start opening");
		}
	} else if((wall_state == 3) && wall_moving){
		if(millis() - t_wall > WALL_TIMEOUT_OPEN){
			sbi(OUTRUT,M1_bk);
			wall_moving = 0;
			wall_state = 0;
			serialPrintLn("Fully oopen");
		}
	}
}


void monederoLoop() {
    if (coin_state == 4){
        coin_state = 0;
        //newCoin();
    }
    /*if(close_wall){
        if(millis() - t_wall > WALL_TIMEOUT){
            cbi(OUTRUT,M1_bk);
			close_wall = 1;
			serialPrintLn("Cierro!");
            open_wall = 1;
        }
    }*/
	checkWall();
    if(led){
        if(millis() - t_led > 1000){
            cbi(OUTRUT,L2);
            led = 0;
        }
    }
}

void monederoSetCallbackCorrecto(void(*f)()) {
    callback = f;
}
