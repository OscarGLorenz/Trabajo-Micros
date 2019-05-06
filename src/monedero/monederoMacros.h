#ifndef MONEDERO_MACROS_H_
#define MONEDERO_MACROS_H_

#include "../commonLibs/macros.h"

#define false 0
#define true 1

// Macros for bit masks
#define BITSHIFT(pos) (1 << (pos))
#define BITSET(reg,pos) (reg |= BITSHIFT(pos))
#define BITCLEAR(reg,pos) (reg &= ~BITSHIFT(pos))
#define BITWRITE(reg,pos,value) (value ? BITSET(reg,pos) : BITCLEAR(reg,pos))
#define BITREAD(reg,pos) ((reg >> pos) & 0x01)

#define readSO2() BITREAD(PIND,1)
#define readSO3() BITREAD(PIND,2)
#define readSW2() BITREAD(PINK,1)
#define writeM1_bk(value) BITWRITE(PINL,6,value)
#define writeM1_en(value) BITWRITE(PINL,4,value)

#endif // MONEDERO_MACROS_H_