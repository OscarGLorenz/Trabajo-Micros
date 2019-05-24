#include "macros.h"
#include "time.h"
#include "pinout.h"
#include "serial.h"

#include "atraccion/atraccion.h"
#include "tarjetero/tarjetero.h"
#include "monedero/monedero.h"

// Numero de pasajeros subidos a la atraccion
int pasajerosListos = 0;
// Estado del pendulo
bool penduloListo = true;

// TarjeteMonedero sutituto
unsigned long debounce = 0;

// Callback de nuevo pasajero
void nuevoPasajero() {
	pasajerosListos++;
}

// Callback de finalización
void finalizado() {
	penduloListo = true;
}

void emergencia() {
    tarjeteroParar();
    monederoParar();
	serialPrintLn("EMERGENCIA!!!!");
}

void setup() {
	// Outputs de la maqueta
	DDR_OUTRUT = 0xFF;

	// Serial
   serialBegin(9600);

	// Time
   initTime();

   // Setups
	atraccionSetup();
    serialPrintLn("Atracción iniciada");

    tarjeteroSetup();
    serialPrintLn("Tarjetero iniciado");

    monederoSetup();
    serialPrintLn("Monedero iniciado");

	// Callbacks
    tarjeteroSetCallbackCorrecto(nuevoPasajero);
  	monederoSetCallbackCorrecto(nuevoPasajero);
	atraccionSetCallbackFinalizado(finalizado);
	atraccionSetCallbackEmergencia(emergencia);
    serialPrintLn("Callbacks iniciados");

}

void loop() {
	atraccionLoop();
    tarjeteroLoop();
    monederoLoop();

	// TarjeteMonedero sutituto
	if (!rbi(PINK,SW3) && millis() - debounce > 500 ) {
		debounce = millis();
		nuevoPasajero();
	}

	if (pasajerosListos >= 1 && penduloListo) {
		atraccionIniciar();
		penduloListo = false;
		pasajerosListos -= 1;
	}

}

int main() {

    setup();

    for(;;)
        loop();

    return 0;
}
