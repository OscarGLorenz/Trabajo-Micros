/***********************************************************************************
*
* FICHERO: main.c
*
* DESCRIPCIÓN :
* 	Gestiona los tres módulos independientes del trabajo. Lleva la cuenta de
* 	personas en la atracción. Además hace inicializaciones varias necesarias para
* 	el funcionamiento de la maqueta.
*
*
* AUTORES:
*
* 	Miguel Fernández Cortizas
*
*   Óscar García Lorenz
*
***********************************************************************************/

// Librerias comunes
#include "macros.h"
#include "time.h"
#include "pinout.h"
#include "serial.h"

// Módulos del trabajo
#include "atraccion/atraccion.h"
#include "tarjetero/tarjetero.h"
#include "monedero/monedero.h"

// Numero de pasajeros subidos a la atracción
int pasajerosListos = 0;
// Estado del pendulo
bool penduloListo = true;

// Variable para limitar pulsaciones en SW3
unsigned long debounce = 0;

// Callback de nuevo pasajero
void nuevoPasajero() {
    pasajerosListos++;
}

// Callback de finalización
void finalizado() {
    penduloListo = true;
}

// Callback de emergencia
void emergencia() {
    // Se paran tarjetero y monedero
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
    monederoSetup();
    serialPrintLn("Monedero iniciado");

    tarjeteroSetup();
    serialPrintLn("Tarjetero iniciado");

    atraccionSetup();
    serialPrintLn("Atracción iniciada");


    // Callbacks
    tarjeteroSetCallbackCorrecto(nuevoPasajero);
    monederoSetCallbackCorrecto(nuevoPasajero);
    atraccionSetCallbackFinalizado(finalizado);
    atraccionSetCallbackEmergencia(emergencia);
    serialPrintLn("Callbacks iniciados");
}

void loop() {
    // Loops de cada parte
    atraccionLoop();
    tarjeteroLoop();
    monederoLoop();

    // Lectura del SW3 para añadir pasajeros manualmente
    if (!rbi(PINK,SW3) && millis() - debounce > 500 ) {
        debounce = millis();
        nuevoPasajero();
    }

    // Cuando hay 4 pasajeros y la atracción esta lista
    if (pasajerosListos >= 4 && penduloListo) {
        // Iniciar atracción y eliminar 4 pasajeros
        atraccionIniciar();
        penduloListo = false;
        pasajerosListos -= 4;
    }

}

int main() {

    setup();

    for(;;)
        loop();

    return 0;
}
