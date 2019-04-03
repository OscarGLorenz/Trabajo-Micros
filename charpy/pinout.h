#ifndef __PINOUT__
#define __PINOUT__

#include <avr/io.h>
#include <avr/interrupt.h>

#define SO1 INT0
#define SO2 INT1
#define SO3 INT2
#define SO4 INT3
#define SO5 PCINT0

#define L1 PL0
#define L2 PL1
#define L3 PL2
#define L4 PL3

#define SW1 PK0 //connected toPCINT16
#define SW2 PK1 //connected toPCINT17
#define SW3 PK2 //connected toPCINT18

#define M1_bk PL6
#define M1_en PL4 //connected to OC5B

#define M2_en PL5 //connected to  OC5C
#define M2_di PL7

#define OUTRUT PORTL

#define cbi(PORT,PIN) (PORT&= ~(1<<PIN))
#define sbi(PORT,PIN) (PORT|=(1<<PIN))


#endif
