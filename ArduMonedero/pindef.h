// File to concentrate port assignment

#ifndef _PINDEF_H
#define _PINDEF_H

#define SOCKET A

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
  #define PIN_26  5
  #define PIN_24  6
  #define PIN_22  7

// Model connector -> Board socket (jumper wires or custom PCB)
#else
/*
  #define PIN_1   COM_1
  #define PIN_2   COM_2
  #define PIN_3   COM_3
  #define PIN_4   COM_4
  #define PIN_5   COM_5
  #define PIN_6   COM_6
  #define PIN_7   COM_7
  #define PIN_8   COM_8
  #define PIN_9   COM_9
  #define PIN_10  COM_10
  #define PIN_11  COM_11
  #define PIN_12  COM_12
  #define PIN_13  COM_13
  #define PIN_14  COM_14
  #define PIN_15  COM_15
  #define PIN_16  COM_16
  #define PIN_17  COM_17
  #define PIN_18  COM_18
  #define PIN_19  COM_19
  #define PIN_20  COM_20
  #define PIN_21  COM_21
  #define PIN_22  COM_22
  #define PIN_23  COM_23
  #define PIN_24  COM_24
  #define PIN_25  COM_25
  #define PIN_26  COM_26
  #define PIN_27  COM_27
  #define PIN_28  COM_28
  #define PIN_29  COM_29
  #define PIN_30  COM_30
  #define PIN_31  COM_31
  #define PIN_32  COM_32
  #define PIN_33  COM_33
  #define PIN_34  COM_34
  */

// Board socket -> ATMEL port
  #if (SOCKET == A)

  #else

  #endif 
#endif

// Naming -> model connector
#define SO2   PIN_21
#define SO3   PIN_23
#define SW2   PIN_31
#define M1_en PIN_26
#define M1_bk PIN_24  
#define L2    PIN_22

#endif // _PINDEF_H
