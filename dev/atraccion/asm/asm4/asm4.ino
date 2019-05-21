//  int aux = (  ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )  - (times[2]+times[3]+times[4])   ; 

volatile uint32_t times[5] = {(uint32_t)2e5, (uint32_t)2e5, (uint32_t)10e5, (uint32_t)1e5, (uint32_t)1e5};
volatile int32_t aux = 0;


void setup() {
  Serial.begin(115200);
    delay(500);

  asm volatile(
    "push r16         \n"
    "push r17         \n"
    "push r18         \n"
    "push r19         \n"
    "push r20         \n"

    "clr r17      \n"
    "clr r18      \n"
    "clr r19      \n"
    "clr r20      \n"

    // times[0]
    "ld r16, X+   \n"
    "add r17, r16 \n"
    "ld r16, X+   \n"
    "adc r18, r16 \n"
    "ld r16, X+   \n"
    "adc r19, r16 \n"
    "ld r16, X+   \n"
    "adc r20, r16 \n"

    // times[0] + times[1]
    "ld r16, X+   \n"
    "add r17, r16 \n"
    "ld r16, X+   \n"
    "adc r18, r16 \n"
    "ld r16, X+   \n"
    "adc r19, r16 \n"
    "ld r16, X+   \n"
    "adc r20, r16 \n"

    // (times[0] + times[1]) << 2
    "rol r17  \n"
    "rol r18  \n"
    "rol r19  \n"
    "rol r20  \n"
    "rol r17  \n"
    "rol r18  \n"
    "rol r19  \n"
    "rol r20  \n"

    // (times[0] + times[1]) << 2 + (times[0] + times[1])
    "sbiw X, 8    \n"
    "ld r16, X+   \n"
    "add r17, r16 \n"
    "ld r16, X+   \n"
    "adc r18, r16 \n"
    "ld r16, X+   \n"
    "adc r19, r16 \n"
    "ld r16, X+   \n"
    "adc r20, r16 \n"
    "ld r16, X+   \n"
    "add r17, r16 \n"
    "ld r16, X+   \n"
    "adc r18, r16 \n"
    "ld r16, X+   \n"
    "adc r19, r16 \n"
    "ld r16, X+   \n"
    "adc r20, r16 \n"
//
//    // ((times[0] + times[1]) << 2 + (times[0] + times[1])) >> 1 = ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )
    "lsr r17  \n"
    "lsr r18  \n"
    "lsr r19  \n"
    "lsr r20  \n"

    // -= (times[2]+times[3]+times[4])
    "ld r16, X+      \n"
    "sub r17, r16    \n"
    "ld r16, X+      \n"
    "sbc r18, r16    \n"
    "ld r16, X+      \n"
    "sbc r19, r16    \n"
    "ld r16, X+      \n"
    "sbc r20, r16    \n"
    
    "ld r16, X+      \n"
    "sub r17, r16    \n"
    "ld r16, X+      \n"
    "sbc r18, r16    \n"
    "ld r16, X+      \n"
    "sbc r19, r16    \n"
    "ld r16, X+      \n"
    "sbc r20, r16    \n"

    "ld r16, X+      \n"
    "sub r17, r16    \n"
    "ld r16, X+      \n"
    "sbc r18, r16    \n"
    "ld r16, X+      \n"
    "sbc r19, r16    \n"
    "ld r16, X+      \n"
    "sbc r20, r16    \n"

    "st Z+,r17       \n"
    "st Z+,r18       \n"
    "st Z+,r19       \n"
    "st Z+,r20       \n"
    
    "pop r20         \n"
    "pop r19         \n"
    "pop r18         \n"
    "pop r17         \n"
    "pop r16         \n"

    ::"x" ((uint16_t)times), "z" ((uint16_t)&aux) 
  );
  
  Serial.println(aux);


}

void loop() {
  // put your main code here, to run repeatedly:

}
