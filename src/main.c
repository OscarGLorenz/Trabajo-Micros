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
bool penduloListo = false;

// Callback de nuevo pasajero
void nuevoPasajero() {
	pasajerosListos++;
	if (pasajerosListos >= 4 && penduloListo) {
		atraccionIniciar();
		penduloListo = false;
		pasajerosListos -= 4;
	}
}

// Callback de finalización
void finalizado() {
	penduloListo = true;
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
	tarjeteroSetup();
	monederoSetup();

	// Callbacks
	tarjeteroSetCallbackCorrecto(nuevoPasajero);
	monederoSetCallbackCorrecto(nuevoPasajero);
	atraccionSetCallbackFinalizado(finalizado);
	
}

void loop() {
	atraccionLoop();
	tarjeteroLoop();
	monederoLoop();
}

int main() {

    setup();

    for(;;)
        loop();

    return 0;
}
