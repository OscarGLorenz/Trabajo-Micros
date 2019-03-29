/**
 * Some of the macros were ported from
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 */

#ifndef _MACROS_H
#define _MACROS_H

//#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
//#define bitSet(value, bit) ((value) |= (1UL << (bit)))
//#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
//#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#include <Arduino.h>

/*
 * Macros to transform STATIC Serial.prints
 * Save a lot of SRAM! Must have at least 1 character
 */
#ifndef Sprint(x)
  #define Sprint(x)       (Serial.print(F(x)))
#endif
#ifndef Sprintln(x)
  #define Sprintln(x)     (Serial.println(F(x)))
#endif

// Macros for bit masks
#undef _BV
#define _BV(b) (1 << (b))
#define TEST(n,b) !!((n)&_BV(b))
#define SBI(n,b) (n |= _BV(b))
#define CBI(n,b) (n &= ~_BV(b))
#define SET_BIT_TO(N,B,TF) do{ if (TF) SBI(N,B); else CBI(N,B); }while(0)

#endif // _MACROS_H
