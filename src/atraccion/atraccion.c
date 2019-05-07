#include "time.h"
#include "atraccion.h"

// Variables globales deben ser estáticas
static void (*callback) ();
static unsigned int tiempo = 0; 

void atraccionLoop() {
	// hago cosas....		
	if (millis() - tiempo > 1000) {
		tiempo = millis();
		callback();
	}
}

void atraccionSetup() {
  // Bla bla
}

void atraccionSetCallbackFinalizado(void(*f)()) {
	callback = f;
}

void atraccionIniciar() {

}
