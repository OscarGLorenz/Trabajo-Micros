#ifndef TARJETERO_H_
#define TARJETERO_H_

void tarjeteroLoop();

void tarjeteroSetup();

void tarjeteroSetCallbackCorrecto(void(*f)());
void tarjeteroSetCallbackError(void(*f)());

void tarjeteroStop();

#endif
