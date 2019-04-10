#ifndef MACROS_H_
#define MACROS_H_

// Clear bit on port
#define cbi(PORT,PIN) (PORT&= ~(1<<PIN))
// Set bit on port
#define sbi(PORT,PIN) (PORT|=(1<<PIN))
// Toggle bit on port
#define tbi(PORT,PIN) (PORT^=(1<<PIN))
// Read bit on port
#define rbi(PORT,PIN) ((PORT & (1<<PIN)) != 0)

#endif
