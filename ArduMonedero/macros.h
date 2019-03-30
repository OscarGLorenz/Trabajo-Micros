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
#define BITSHIFT(pos) (1 << (pos))
#define BITSET(reg,pos) (reg |= BITSHIFT(pos))
#define BITCLEAR(reg,pos) (reg &= ~BITSHIFT(pos))
#define BITWRITE(reg,pos,value) (value ? BITSET(reg,pos) : BITCLEAR(reg,pos))
#define BITREAD(reg, pos) ((reg >> pos) & 0x01)

#endif // _MACROS_H
