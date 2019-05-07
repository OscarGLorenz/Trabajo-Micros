#ifndef MONEDERO_CONFIG_H_
#define MONEDERO_CONFIG_H_

//#include "../commonLibs/pinout.h"
//#include "monederoMacros.h"   // For macros, some of them ported from Marlin

void monederoConfig() {
	while (rbi(PINK,SW1));
	DDRL = 0xFF;		// Configure output port
	cli();
	// Interrupt triggers
	BITSET(EICRA,ISC10);
	BITCLEAR(EICRA,ISC11);
	BITSET(EICRA,ISC20);
	BITCLEAR(EICRA,ISC21);
	
	//Interrupt enable
	BITSET(EIMSK,INT1);
	BITSET(EIMSK,INT2);
	sei();
}

#endif  // MONEDERO_CONFIG_H