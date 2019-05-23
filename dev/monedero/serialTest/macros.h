#ifndef MACROS_H_
#define MACROS_H_

#define true 1
#define false 0

#define mbi(pos) (1 << (pos))
#define rbi(reg,bit) ((reg >> bit) & 0x01)  	// Read bit of register
#define sbi(reg,bit) (reg |= mbi(bit))  		// Set bit of register
#define cbi(reg,bit) (reg &= ~mbi(bit)) 		// Clear bit of register
#define tbi(reg,bit) (reg ^= mbi(bit))  		// Toggle bit of register
#define wbi(reg,bit,value) (value ? sbi(reg,bit) : cbi(reg,bit))		// Write value in bit of register

#endif // MACROS_H_
