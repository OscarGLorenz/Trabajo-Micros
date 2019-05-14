#ifndef ATRACCION_H_
#define ATRACCION_H_

void atraccionLoop();

void atraccionSetup();

void atraccionSetCallbackFinalizado(void(*f)());
void atraccionSetCallbackFuncionamiento(void(*f)());
void atraccionSetCallbackStop(void(*f)());

void atraccionIniciar();

#endif
