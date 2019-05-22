// File to concentrate port assignment

#ifndef _PINDEF_H
#define _PINDEF_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Auto detect board type
#if defined(__AVR_ATmega328P__)
  #define BOARD_TYPE UNO
#elif defined(__AVR_ATmega2560__)
  #define BOARD_TYPE MEGA
#else
  #define BOARD_TYPE MOLASE
#endif

// Model connector -> Arduino
#if (BOARD_TYPE == UNO) || (BOARD_TYPE == MEGA)
  #define PIN_21  2
  #define PIN_23  3
  #define PIN_31  4
  #define PIN_26  8
  #define PIN_24  9
  #define PIN_22  10

// Model connector -> MCU port
#else   // Board is MOLASE, socket A
  #define PIN_21  PINL,2
  #define PIN_23  PINL,4
  #define PIN_31  PIND,4
  #define PIN_26  PORTL,7
  #define PIN_24  PORTL,5
  #define PIN_22  PORTL,3
#endif

// Naming -> model connector
#define SO2   PIN_21
#define SO3   PIN_23
#define SW2   PIN_31
#define M1_en PIN_26
#define M1_bk PIN_24  
#define L2    PIN_22

#endif // _PINDEF_H
