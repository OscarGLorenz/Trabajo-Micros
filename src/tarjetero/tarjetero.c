#include "time.h"
#include "tarjetero.h"

// Variables globales deben ser estáticas
static void (*callback) ();
static unsigned int tiempo = 0; 

void tarjeteroLoop() {
	// hago cosas....		
	if (millis() - tiempo > 1000) {
		tiempo = millis();
		callback(); // Persona correcta
	}
}

void tarjeteroSetup() {
  // Bla bla
}

void tarjeteroSetCallbackCorrecto(void(*f)()) {
	callback = f;
}

