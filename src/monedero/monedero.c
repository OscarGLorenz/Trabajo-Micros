#include "time.h"
#include "monedero.h"

// Variables globales deben ser estáticas
static void (*callback) ();
static unsigned int tiempo = 0; 

void monederoLoop() {
	// hago cosas....		
	if (millis() - tiempo > 1000) {
		tiempo = millis();
		callback();  // Persona correcta
	}
}

void monederoSetup() {
  // Bla bla
}

void monederoSetCallbackCorrecto(void(*f)()) {
	callback = f;
}

void monederoParar() {

}