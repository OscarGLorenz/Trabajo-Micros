#ifndef PINOUT_H_
#define PINOUT_H_

#include <avr/io.h>
#include <avr/interrupt.h>


// OPTIC SENSORS
#define SO1 INT0 	// 27 connected to PD0
#define SO2 INT1 	// 28 connected to PD1
#define SO3 INT2 	// 29 connected to PD2
#define SO4 INT3 	// 30 connected to PD3
#define SO5 PCINT0 	// 1  connected to PB0

// LEDS
#define L1 PL0 		// 19
#define L2 PL1 		// 20
#define L3 PL2 		// 21
#define L4 PL3 		// 22

// MICROSWITCHES
#define SW1 PK0 	//9  connected to PCINT16
#define SW2 PK1 	//10 connected to PCINT17
#define SW3 PK2 	//11 connected to PCINT18

// MOTORS
#define M1_en PL4 	// 23 connected to OC5B
#define M2_en PL5 	// 24 connected to OC5C

#define M1_bk PL6 	// 25
#define M2_di PL7 	// 26



#define OUTRUT PORTL
#define OUTRUT_DDR DDRB

#define PINRUT PINB

#endif
