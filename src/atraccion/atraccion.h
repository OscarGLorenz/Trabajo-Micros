/*************************************************************
*
* FICHERO: atraccion.h
*
* DESCRIPCIÓN :
* 	Contiene la funcionalidad de la atracción. Implementada como
*	máquina de estados
*
*
* FUNCIONES :
*
*   void atraccionLoop():
*      Esta función debe ser llamada continuamente
*      contiene el grueso del programa y la funcionalidad
*
*   void atraccionSetup():
*      Esta función debe ser llamada al comienzo del programa
*      inicializa los perifericos del micro y inicializa lo
*      necesario
*
*   void atraccionIniciar():
*      Esta función debe ser llamada cuando se desea iniciar
*      la atracción. Solo funcionará si esta no esta funcionando
*      ni en modo emergencia
*
*   void atraccionSetCallbackFinalizado(void(*f)()):
*      Con esta función se le da a atracción la función que
*      el programa principal llamará cuando la atracción haya
*      terminado su proceso y esté lista para otro viaje
*
*   void atraccionSetCallbackEmergencia(void(*f)()):
*      Con esta función se le da a atracción la función que
*      el programa llamará cuando la atracción haya entrado
*      en modo emergencia pulsando SW1
*
*************************************************************/

#ifndef ATRACCION_H_
#define ATRACCION_H_

void atraccionLoop();

void atraccionSetup();

void atraccionSetCallbackFinalizado(void(*f)());
void atraccionSetCallbackFuncionamiento(void(*f)());
void atraccionSetCallbackStop(void(*f)());

void atraccionIniciar();

void atraccionSetCallbackEmergencia(void(*f)());

#endif
