/*
 * Monedero_simple.c
 *
 * Created: 08/05/2019 17:45:48
 * Author : marta_000
 */ 

#include <avr/io.h>
#include "monedero.h"


int main(void)
{
    monederoSetup();
    while (1) 
    {
		monederoLoop();
    }
}

