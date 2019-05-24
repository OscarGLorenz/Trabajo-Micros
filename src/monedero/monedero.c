/***********************************************************************************
*
* FICHERO: monedero.c
*
* DESCRIPCIÓN :
* 	Contiene la funcionalidad del monedero:
*
*	1 -	Posibilidad de calibrado de monedas los 15 primeros seg de programa. 
*	2 - Obtención de los valores del tiempo en las interruciones de los sensores.
*	3 - Cálculo del ratio d/s y comparación con los rangos guardados para
*		distinguir qué moneda es.
*	4 - Aceptación o rechazo de la moneda. 
*	5 - En caso de aceptarse, se incrementa el valor del dinero pagado en el valor 
*		que corresponda. Cuando éste sea >= 120 c, se notifica al programa principal.
*
* AUTORES:
*
*	Marta Centeno Tortajada
*
*	Mario Musicò Cortés
*
***********************************************************************************/

#include <string.h>
//#include <stdbool.h>

#include "monedero.h"

#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"

// Flags de precompilador para habilitar depuración por serial de variables relevantes para la detección
//#define SERIAL_DEBUG
//#define SHOW_RATIOS


// Macros para máscaras de lectura de los sensores
#define readSO2() rbi(PIND,INT1)
#define readSO3() rbi(PIND,INT2)
#define readSW2() rbi(PINK,PCINT17)


#define CALIB_TIMEOUT 15000    // Tiempo para elegir la opción de calibrado al comienzo
#define WALL_TIMEOUT 2         // Tiempo que está la barrera cerrada


// Declaración de callback de nueva persona para su llamada
static void (*callback) ();

// Variables de la moneda activa para calibración
static uint8_t coin_id; 

struct coin_params {
    uint16_t ds_min;
    uint16_t ds_max;
};

static struct coin_params coins[8];  // Ratio máximo y mínimo de cada moneda para identificación, es un array de estructuras con campos max y min para cada moneda

// Variables de detección NO ESTÁTICAS que usa ASM
unsigned long t_coin;         // Para guardar el tiempo cuando saltan las interrupciones de S02 y S03
unsigned long t2u, t3u, t2d;  // ASM guarda t_coin en la variable de tiempo del sensor y flanco correspondiente (up, down)
uint8_t coin_state;           // Antirruido, para comprobar que no son medidas falsas
uint8_t pin_sensor;           // Valor de puerto D cuando salta la interrución

// Variables de detección estáticas para calcular el ratio
static unsigned long d;
static unsigned long s;

// Flags para el módulo de monedero
static uint8_t led;
static uint8_t calibrate;
static uint8_t lobotomy;
static uint8_t wall;

// Dinero acumulado en el pago en curso
static uint8_t payment_money;

// Variable para guardar el tiempo de cierre de barrera para su reapertura pasado su tiemout 
static unsigned long t_close;
// Variable para guardar el tiempo de encendido de L2 para su apagado pasado su timeuot
static unsigned long t_led;


// Función que inicializa los ratios d/s máximo y mínimo de cada moneda, la ponemos arriba para reajustar rápidamente
void loadDefaultLimits(){
    coins[0].ds_min = 1000; coins[0].ds_max = 0;		// 1c coin, no se usa 
    coins[1].ds_min = 438; coins[1].ds_max = 514;		// 2c coin
    coins[2].ds_min = 250; coins[2].ds_max = 270;		// 5c coin
    coins[3].ds_min = 328; coins[3].ds_max = 376;		// 10c coin
    coins[4].ds_min = 216; coins[4].ds_max = 230;		// 20c coin
    coins[5].ds_min = 175; coins[5].ds_max = 185;		// 50c coin
    coins[6].ds_min = 190; coins[6].ds_max = 205;		// 100c coin
    coins[7].ds_min = 155; coins[7].ds_max = 162;		// 200c coin
}

void newCoin();  // Declaración de función para ser llamada durante las interrupciones

// ###################  RUTINAS DE INTERRUPCIÓN EN ASM ########################

extern "C" void asm_ISR_SO2();
extern "C" void asm_ISR_SO3();

// ISR sensor S02
ISR(SO2_vect){   
        pin_sensor = PIND;
        t_coin = micros();
        asm_ISR_SO2();  // Llamada a función ASM
        if(coin_state == 3) newCoin();  // Si ha pasado por todas las etapas, es una moneda y no ruido, comparar y actuar
}

// ISR sensor S03
ISR(SO3_vect){   
        pin_sensor = PIND;
        t_coin = micros();
        asm_ISR_SO3();  // Llamada a función ASM

}

// ###################  RUTINAS DE INTERRUPCIÓN EN C ##########################

// ISR switch SW2
ISR(SW2_vect){
        sbi(OUTRUT,M1_bk);       // Parar el motor encendiendo el freno
        if(readSW2()&&(!lobotomy)){  //Si se ha cerrado la barrera y no es una parada de emergencia
            t_close = millis();      // Se volverá a abrir la puerta en el loop
            wall = 1;
        } else if (lobotomy){     // Si hay parada de emergencia se deshabilitan interrupciones, la barrera se queda cerrada
            cbi(EIMSK,INT1);	
            cbi(EIMSK,INT2);
            cbi(PCICR, PCIE2);
        } else {                // Se ha abierto la barrera
            wall = 0;
        }
}


// Interrupciones en C de S02 y S03, no descomentar salvo fallo de ASM
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

// ################### Funciones de modo CAL ##############################

void printLimitsCoin(){      // Imprimir los límites de la moneda que se está calibrando
    serialPrint("Now coin ID = ");
    serialPrintInt(coin_id);
    serialPrintLn(" ratios limits are:");
    serialPrint("d/s min:\t"); serialPrintInt(coins[coin_id].ds_min); serialPrint("\n");
    serialPrint("d/s max:\t"); serialPrintInt(coins[coin_id].ds_max); serialPrint("\n");
    serialPrint("\n");
}

void printLimitsAll(){   // Imprimir los límtines de todas las monedas
    serialPrintLn("Now coin ratios limits are:");
    serialPrintLn("Coin type:\t0(1c)\t1(2c)\t2(5c)\t3(10c)\t4(20c)\t5(50c)\t6(1e)\t7(2e)");
    serialPrintLn("\t\t-------------------------------------------------------------");
    serialPrint("d/s min:\t");
    for(int i = 0; i<8 ; i++) {
        serialPrintInt(coins[i].ds_min);
        serialPrint("\t");
    }
    serialPrint("\nd/s max:\t");
    for(int i = 0; i<8 ; i++) {
        serialPrintInt(coins[i].ds_max);
        serialPrint("\t");
    }
    serialPrintLn("\n");
}

void resetLimit(struct coin_params* coin){     // Reinicia los límites del ratio d/s para la moneda activa en coin_id para resetear su calibración
    serialPrint("Resetting limits of coin id= ");
    serialPrintInt(coin_id);
    serialPrint("\n\n");
    coin->ds_min = 100;
    coin->ds_max = 0.0;
}

void adjustCoin(uint16_t ds){           // Reajuste de los límites de la moneda activa en coin_id en el modo calibración
    serialPrint("Last coin ds ratio is: ");
    serialPrintInt(ds);
    serialPrint("\n");
    if(coins[coin_id].ds_min > ds) coins[coin_id].ds_min = ds;  // Si el ratio obtenido es más pequeño que el mínimo, se sustituye
    if(coins[coin_id].ds_max < ds) coins[coin_id].ds_max = ds;  // Si el ratio obtenido es más grande que el máximo, se sustituye
    printLimitsCoin();
}

// ################### Funciones de etapa de inicio ##############################

void serialWelcome(){   // Opciones disponibles
    serialPrintLn("Available commands:");
    serialPrintLn("cal - Calibrate ratios limits");
    serialPrintLn("run - Exit calibration mode");
    serialPrintLn("all - Print all ratios limits");
    serialPrintLn("When in CAL mode:");
    serialPrintLn("\t[value in cents] of coin");
    serialPrintLn("\tres restarts coin limits");
    printLimitsAll();
}

// Watchdog los primeros 15 segundos para habilitar la calibración y salir de ella
void serialWatchdog(){
    char command[5]="";
    if (rbi(RX_CHECK_REG, RX_CHECK_BIT)){
        serialReadString(command, 2000);
        serialPrint(command);
        if(!strcmp(command,"cal")) { serialPrintLn(": CALIBRATE MODE"); calibrate = 1; }
        else if (!strcmp(command,"run")) { serialPrintLn(": RUN MODE"); calibrate = 2; }
        else if (!strcmp(command,"res")) { serialPrintLn(": RESET COIN LIMITS"); resetLimit(&coins[coin_id]); }
        else if (!strcmp(command,"all")) { serialPrintLn(": RESET COIN LIMITS"); printLimitsAll(); }
        else if (!strcmp(command,"2")) { serialPrintLn(": configuring 2c coin"); coin_id = 1; }
        else if (!strcmp(command,"5")) { serialPrintLn(": configuring 5c coin"); coin_id = 2; }
        else if (!strcmp(command,"10")) { serialPrintLn(": configuring 10c coin"); coin_id = 3; }
        else if (!strcmp(command,"20")) { serialPrintLn(": configuring 20c coin"); coin_id = 4; }
        else if (!strcmp(command,"50")) { serialPrintLn(": configuring 50c coin"); coin_id = 5; }
        else if (!strcmp(command,"100")) { serialPrintLn(": configuring 100c coin"); coin_id = 6; }
        else if (!strcmp(command,"200")) { serialPrintLn(": configuring 200c coin"); coin_id = 7; }
        else serialPrintLn(": error, command not available");
    }
}

// ################### Funciones de modo RUN ################################

void coinAccepted(uint8_t cents){   	// Si el ratio es válido, se acepta la moneda
    payment_money += cents;           	// Se añade el valor de la moneda al total pagado
    serialPrint("\nCoin of ");
    serialPrintInt(cents);
    serialPrintLn(" added to payment, total inserted money: ");
    serialPrintInt(payment_money);
    serialPrintLn(" cents.\n");
    if (payment_money >= 120){         // Si el total pagado es >= 120, se añade una persona
        callback();
        sbi(OUTRUT,L2);             // Se enciende el led 1 segundo
        t_led = millis();
        led = 1;
        serialPrint("***** New payment completed: ");
        serialPrintInt(payment_money);
        serialPrintLn(" *****");
        payment_money = 0;          // Total pagado es cero, si había más de 120 cent se pierden
    }
    serialPrint("\n");

}

void compareCoin(uint16_t ds){
    uint8_t cents;
    if ((ds >= coins[3].ds_max) || (ds <= coins[5].ds_min)) {  	// Pre-comparación para abrir el motor más rápido, ya que mirando los ratios se pueden descartar todas en ese intervalo
        cbi(OUTRUT,M1_bk);										// Deshabilitar el freno del motor para que se mueva la barrera y se cierre, se parará cuando la interrupción por pin change en SW2 detecte que ha sido pulsado
    }
    if ((ds >= coins[2].ds_min) && (ds <= coins[2].ds_max)) {cents = 5; cbi(OUTRUT,M1_bk);}     // El caso de la moneda de 5c se trata aparte ya que se encuentra entre la de 10c y la de 20c que no se tienen que rechazar
    else if ((ds >= coins[1].ds_min) && (ds <= coins[1].ds_max)) cents = 2;     // 2c coin
    else if ((ds >= coins[7].ds_min) && (ds <= coins[7].ds_max)) cents = 200;   // 200c coin
    else if ((ds >= coins[6].ds_min) && (ds <= coins[6].ds_max)) cents = 100;   // 100c coin
    else if ((ds >= coins[5].ds_min) && (ds <= coins[5].ds_max)) cents = 50;    // 50c coin
    else if ((ds >= coins[4].ds_min) && (ds <= coins[4].ds_max)) cents = 20;    // 20c coin
    else if ((ds >= coins[3].ds_min) && (ds <= coins[3].ds_max)) cents = 10;    // 10c coin
    else cents = 1;                                                             // Si no se ha reconocido se le asigna 1c

    if ((cents < 10) || (cents > 100)){    	// Monedas de 1, 2, 5, 200 c rechazadas
        serialPrintLn("\nCoin rejected, sorry.");
        cbi(OUTRUT,M1_bk);					// Freno del motor apagado para que se mueva la barrera, no se ha reconocido como moneda de 1c, 2c, 5c pero tampoco de las de los rangos aceptados, por lo que se rechaza para evitar sumar un valor dudoso
    } else {
        coinAccepted(cents);                // Monedas de 10, 20, 50, 100 c aceptadas
    }
	
	#ifdef SERIAL_DEBUG
		serialPrint("\nwd/s =");
		serialPrintInt(ds);
		serialPrint("\n");
	#endif
}

void newCoin(){   // Nueva moneda detectada
    uint16_t ds;
    //float ds;		// Previamente usábamos comparación con floats pero multiplicándolos por 100 y tratándolos como entero es mucho más rápido y consume menos recursos
    d = t3u - t2u;  // Cálculo de la diferencia entre entradas en los sensores
    s = t2d - t3u;  // Cálculo del solapamiento entre los dos sensores
    ds = (uint16_t)(d * 100 / s);  // Ratio calculado como d/s
    //ds = (float) d / s;          // Mejor usar división entera que con float
	
	#ifdef SHOW_RATIOS
		serialPrint("New coin introduced:\t");
		serialPrint("d = "); serialPrintInt(d);
		serialPrint("\td = "); serialPrintInt(s);
		serialPrint("\td/s = "); serialPrintInt(ds);
	#endif
	
    if (calibrate == 1) compareCoin(ds);  	// Si no está en modo calibración se identifica la moneda ESTO ES LO QUE ESTABA MAL EN LA DEMOSTRACIÓN!!
    else adjustCoin(ds);              		// Si está en modo calibración se reajustan los límites
}



// ################### Funcions principales del módulo #########################

void monederoParar(){	// Lobotomiza el monedero, activa la barrera para que se cierre y la rutina de interrupción del switch la apague estando cerrada
    serialPrintLn("\nMonedero out of service :(");
    cbi(OUTRUT,M1_bk);
    lobotomy = 1;
}

void monederoConfig() {   // Configuración de interrupciones para el módulo de monedero mediante máscaras
    cli();
    sbi(EICRA,ISC10);   // INT1 
    cbi(EICRA,ISC11);
    sbi(EICRA,ISC20);	// INT2 
    cbi(EICRA,ISC21);
    sbi(EIMSK,INT1);	// Habilitación de INT1 INT2
    sbi(EIMSK,INT2);
    sbi(PCMSK2,PCINT17);	// Pin change SW2
    sbi(PCICR, PCIE2);
    sei();
}

void initWall(){          // Dejar barrera inicialmente abierta si estuviese cerrada
    //serialPrintInt(readSW2());
    if(readSW2()){
	#ifdef SERIAL_DEBUG
        serialPrintLn("Still closed");
	#endif
        cbi(OUTRUT,M1_bk);
    }
}

void monederoSetup() {
    monederoConfig();		// Configuración de puertos
    loadDefaultLimits();	// Inicializar límites de ratio d/s de array coins[8]
    initWall();		        // Dejar barrera inicialmente abierta si estuviese cerrada
    serialWelcome();		// Visualizar comandos disponibles

	// Inicialización de flags para el módulo de monedero
    calibrate = false;
    coin_state = 0;
    coin_id = 8;
    led = 0;
    payment_money = 0;
    wall = 0;

    while(((CALIB_TIMEOUT - millis()) > 0) && (calibrate != 2)) serialWatchdog();		// Dar la opción de entrar en modo calibración durante 15 segundos, salvo que se haya recibido el comando RUN
    while(calibrate == 1) serialWatchdog();												// Mantener activo el modo calibración si se ha recibido el comando CAL
    serialPrintLn("Monedero is running\n");
}

void monederoLoop() {
    if(wall){                 // Volver a abrir barrera después de rechazar moneda, por consulta a millis()
        if(millis() - t_close > WALL_TIMEOUT){
            cbi(OUTRUT,M1_bk);
        }
    }
    if(led){                 // Apagar L2 después de haberlo encendido durante 1s, por consulta a millis()
        if(millis() - t_led > 1000){
            cbi(OUTRUT,L2);
            led = 0;
        }
    }
}

// Configuración de callback para añadir nueva persona a la cola cuando se ha validado pago
void monederoSetCallbackCorrecto(void(*f)()) {
    callback = f;
}