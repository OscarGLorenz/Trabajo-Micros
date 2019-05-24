//  int aux = (  ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )  - (times[2]+times[3]+times[4])   ; 
#include <avr/io.h>
volatile uint32_t times[5] = {(uint32_t)2e5, (uint32_t)2e5, (uint32_t)10e5, (uint32_t)1e5, (uint32_t)1e5};
volatile int32_t aux = 0x0FFFFFFF;


void setup() {
  Serial.begin(115200);
    delay(500);


  asm volatile(
    "sbi %0, %1 \n" //pinMode(13, OUTPUT);


    
    "push r16         \n"
    "push r17         \n"
    "push r18         \n"
    "push r19         \n"
    "push r20         \n"

    "ld r17, Z+    \n"
    "ld r18, Z+    \n"
    "ld r19, Z+    \n"
    "ld r20, Z+    \n"

    "cpi r20, 0 \n"
    "brne nozero \n"
    "cpi r19, 0 \n"
    "brne nozero \n"
    "cpi r18, 0 \n"
    "brne nozero \n"
    "cpi r17, 0 \n"
    "brne nozero \n"
    "sivale:     \n"
    
    
    "sbi %2, %1 \n" //pinMode(13, OUTPUT);


    "jmp end \n"
    
    "nozero: \n"
    "andi r20,0x80 \n"
    "cpi r20, 0x80 \n"
    "breq sivale \n"

    "cbi %2, %1 \n" //pinMode(13, OUTPUT);

    "end:\n"
  
    "pop r20         \n"
    "pop r19         \n"
    "pop r18         \n"
    "pop r17         \n"
    "pop r16         \n"
    ::  "I" (_SFR_IO_ADDR(DDRB)), "I" (DDB5), "I" (_SFR_IO_ADDR(PORTB)),"z" ((uint16_t)&aux)
  );
  
  Serial.println(aux);


}

void loop() {
  // put your main code here, to run repeatedly:

}
