/*************************************************************
*
* FICHERO: pinout.h
*
* DESCRIPCIÓN :
* 	Macros de la interconexión de la maqueta
*
*************************************************************/

#ifndef PINOUT_H_
#define PINOUT_H_

#include <avr/io.h>
#include <avr/interrupt.h>


// Sensores ópticos por interrupción externa
#define SO1 INT0 	// 27 conectado a PD0
#define SO2 INT1 	// 28 conectado a PD1
#define SO3 INT2 	// 29 conectado a PD2
#define SO4 INT3 	// 30 conectado a PD3

// Interrupciones de S01-S04
#define INT_MASK EIMSK
#define CTRL_INT EICRA
#define SO1_C0 ISC00
#define SO1_C1 ISC01
#define SO2_C0 ISC10
#define SO2_C1 ISC11
#define SO3_C0 ISC20
#define SO3_C1 ISC21
#define SO4_C0 ISC30
#define SO4_C1 ISC31
#define SO1_vect INT0_vect
#define SO2_vect INT1_vect
#define SO3_vect INT2_vect
#define SO4_vect INT3_vect


// Configuración de la interrupción de S05 (PCINT)
#define SO5 PCINT0 	// 1 conectado a PB0
#define PORT_S05 PORTB
#define P_S05 PB0
#define PIN_S05 PINB
#define SO5_MASK PCMSK0
#define SO5_ENABLE PCIE0
#define S05_CTRL PCICR
#define SO5_vect PCINT0_vect

// PCINT SW2
#define SW2_vect PCINT2_vect


// LEDS
#define DDR_OUTRUT DDRL
#define OUTRUT PORTL
#define L1 PL0 		// 19
#define L2 PL1 		// 20
#define L3 PL2 		// 21
#define L4 PL3 		// 22
// MOTORES
#define M1_en PL4 	// 23 conectado a OC5B
#define M2_en PL5 	// 24 conectado a OC5C
#define M1_bk PL6 	// 25
#define M2_di PL7 	// 26


// MICROINTERRUPTORES
#define PINRUT PINK
#define SW1 PK0 	//9  conectado a PCINT16
#define SW2 PK1 	//10 conectado a PCINT17
#define SW3 PK2 	//11 conectado a PCINT18

#endif
