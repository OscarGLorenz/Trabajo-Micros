#ifndef __PINOUT__
#define __PINOUT__

#include <avr/io.h>
#include <avr/interrupt.h>

#define SO1 INT0 // 27
#define SO2 INT1 // 28
#define SO3 INT2 // 29
#define SO4 INT3 // 30
#define SO5 PCINT0 // 1

#define L1 PL0 // 19
#define L2 PL1 // 20
#define L3 PL2 // 21
#define L4 PL3 // 22

#define SW1 PK0 //9 connected toPCINT16
#define SW2 PK1 //10 connected toPCINT17
#define SW3 PK2 //11 connected toPCINT18

#define M1_en PL4 // 23 connected to OC5B
#define M2_en PL5 // 24 connected to OC5C

#define M1_bk PL6 // 25
#define M2_di PL7 // 26

#define OUTRUT PORTL

#define cbi(PORT,PIN) (PORT&= ~(1<<PIN))
#define sbi(PORT,PIN) (PORT|=(1<<PIN))


#endif
