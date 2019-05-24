
/*************************************************************
*
* FICHERO: time.h
*
* DESCRIPCIÓN :
* 	Libreria que permite medir tiempos en microsegundos y en milisegundos empleando el TIMER1.
*
* FUNCIONES:
*
*		unsigned long millis():
*			Devuelve los milisegundos que han pasado desde el inicio del programa.
*
*		unsigned long micros():
*			Devuelve los microsegundos que han pasado desde el inicio del programa.
*
*		void initTime():
*			Inicializa el timer para poder usar millis() y micros()
*
*
************************************************************/


#ifndef TIME_H_
#define TIME_H_

unsigned long millis();

unsigned long micros();

void initTime();

#endif //TRABAJO_MICROS_TIME_H
