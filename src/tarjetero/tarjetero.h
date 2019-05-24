/*************************************************************
*
* FICHERO: tarjetero.h
*
* DESCRIPCIÓN :
* 	Contiene la funcionalidad del tarjetero. Implementada en 3 fases:
*
*
* FUNCIONES :
*
*   void tarjeteroLoop():
*      Esta función debe ser llamada continuamente.
*      Contiene el grueso del programa y la funcionalidad
*
*   void tarjeteroSetup():
*      Esta función debe ser llamada al comienzo del programa
*      inicializa los perifericos del micro e inicializa lo
*      necesario
*
*
*   void atraccionSetCallbackCorrecto(void(*f)()):
*      Con esta función se le da a tarjetero la función a la que
*      deberá llamar cuando se detecte una coincidencia buena con
*      la tarjeta
*
*   void tarjeteroParar():
*      Función que el programa principal llamará cuando sea pulsado
*      el botón de emergencia. Esto inhabilita el tarjetero.
*      
*
*************************************************************/



#ifndef TARJETERO_H_
#define TARJETERO_H_

void tarjeteroLoop();

void tarjeteroSetup();

void tarjeteroSetCallbackCorrecto(void(*f)());

void tarjeteroParar();

#endif
