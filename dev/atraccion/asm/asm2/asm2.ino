volatile uint32_t now;
volatile uint32_t past=1;

void setup() {
  Serial.begin(115200);
    now= 2e6;
  past=1e2;
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(10);
 
  asm volatile(
  "push r26 \n"
  "push r27 \n"
  "push r30 \n"
  "push r31 \n"
  "push r16 \n"
  "push r17 \n"
  
  "ld r16,X \n"
  "ld r17,Z+ \n"
  "sub r16,r17\n"
  "st X+,r16\n"
  
  "ld r16,X \n"
  "ld r17,Z+ \n"
  "sbc r16,r17\n"
  "st X+,r16\n"
  
  "ld r16,X \n"
  "ld r17,Z+ \n"
  "sbc r16,r17\n"
  "st X+,r16\n"
  
  "ld r16,X \n"
  "ld r17,Z+ \n"
  "sbc r16,r17\n"
  "st X+,r16\n"
  
  "pop r17 \n"
  "pop r16 \n"
  "pop r31 \n"
  "pop r30 \n"
  "pop r27 \n"
  "pop r26 \n"
  
  ::"x" (&now),"z" (&past)
  );
  
  Serial.println(" now= " + String(now)+ " past= " + String(past));


}
