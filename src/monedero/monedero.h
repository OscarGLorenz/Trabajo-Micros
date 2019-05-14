#ifndef MONEDERO_H_
#define MONEDERO_H_

void monederoLoop();

void monederoSetup();

void monederoSetCallbackCorrecto(void(*f)());
void monederoSetCallbackMoneda(void(*f)());
void monederoSetCallbackError(void(*f)());

void monederoStop();


#endif
