/*************************************************************
*
* FICHERO: atraccion.h
*
* DESCRIPCIÓN :
* 	Varias macros para el manejo de registros bit a bit
*
*************************************************************/

#ifndef MACROS_H_
#define MACROS_H_

#define true 1
#define false 0

#define mbi(pos) (1 << (pos))
#define rbi(reg,bit) ((reg >> bit) & 0x01)  	// Leer bit de reg
#define sbi(reg,bit) (reg |= mbi(bit))  		// Poner a 1 bit de reg
#define cbi(reg,bit) (reg &= ~mbi(bit)) 		// Poner a 0 bit de reg
#define tbi(reg,bit) (reg ^= mbi(bit))  		// Cambiar de valor bit de reg

#endif // MACROS_H_
