#include "time.h"
#include "monedero.h"

// CALLBACKS
static void (*nuevaPersona) ();
static void (*moneda) ();
static void (*noMoneda) ();
void monederoSetCallbackCorrecto(void(*f)()) {
	nuevaPersona = f;
}
void monederoSetCallbackMoneda(void(*f)()) {
	moneda = f;
}
void monederoSetCallbackError(void(*f)()) {
	noMoneda = f;
}
// CALLBACKS


static unsigned int tiempo = 0; 

void monederoLoop() {
	// hago cosas....		
	if (millis() - tiempo > 1000) {
		tiempo = millis();
		nuevaPersona();  // Persona correcta
	}
}

void monederoSetup() {
  // Bla bla
}


void monederoStop() {
	// Mucho malo idiota
}