/**
 * Some of the macros were ported from
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 */

#ifndef MACROS_H_
#define MACROS_H_

// Macros for bit masks
#define BITSHIFT(pos) (1 << (pos))
#define BITSET(reg,pos) (reg |= BITSHIFT(pos))
#define BITCLEAR(reg,pos) (reg &= ~BITSHIFT(pos))
#define BITWRITE(reg,pos,value) (value ? BITSET(reg,pos) : BITCLEAR(reg,pos))
#define BITREAD(reg, pos) ((reg >> pos) & 0x01)

/*
 * Macros to transform STATIC Serial.prints
 * Save a lot of SRAM! Must have at least 1 character
 */
#ifdef Arduino_h
  #ifndef Sprint(x)
    #define Sprint(x) (Serial.print(F(x)))
  #endif
  #ifndef Sprintln(x)
    #define Sprintln(x) (Serial.println(F(x)))
  #endif
#endif

// Clear bit on port
#define cbi(PORT,PIN) (PORT&= ~(1<<PIN))
// Set bit on port
#define sbi(PORT,PIN) (PORT|=(1<<PIN))
// Toggle bit on port
#define tbi(PORT,PIN) (PORT^=(1<<PIN))
// Read bit on port
#define rbi(PORT,PIN) ((PORT & (1<<PIN)) != 0)

#endif // MACROS_H_
