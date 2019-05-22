#ifndef ATRACCION_H_
#define ATRACCION_H_

void atraccionLoop();

void atraccionSetup();

void atraccionSetCallbackFinalizado(void(*f)());

void atraccionIniciar();

void atraccionSetCallbackEmergencia(void(*f)());

#endif
