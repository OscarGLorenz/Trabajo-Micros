#include "time.h"
#include "tarjetero.h"

// CALLBACKS
static void (*nuevaPersona) ();
static void (*error) ();
void tarjeteroSetCallbackCorrecto(void(*f)()) {
	nuevaPersona = f;
}
void tarjeteroSetCallbackError(void(*f)()) {
	error = f;
}
// CALLBACKS


static unsigned int tiempo = 0; 

void tarjeteroLoop() {
	// hago cosas....		
	if (millis() - tiempo > 1000) {
		tiempo = millis();
		nuevaPersona(); // Persona correcta
	}
}

void tarjeteroSetup() {
  // Bla bla
}



void tarjeteroStop() {
	// Mucho malo mucho tonto
}
