uint8_t dir = 1;
int16_t pos = 0;


void setup() {

  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:

 delay(10);
  
  Serial.println("pos = " + String(pos) + " dir= " + String(dir));

  asm volatile(
  " push r16\n"
  " mov r16,%1\n"
  " tst r16 \n"
  " breq zero \n"
  //restar 1 
  
  "ldi r16, 1 \n" 
  "sub %B0, r16 \n"
  "clr r16 \n"
  "sbc %B0, r16 \n"
  
  " jmp end\n"
  "zero: \n"
  //sumar 1
  "ldi r16, 1 \n" 
  "add %A0, r16 \n"
  "clr r16 \n"
  "adc %B0, r16 \n"
  "end: \n"
  
  //" \n"
  
  "pop r16 \n"
  

  :"+r" (pos) :"r" (dir):
  );

  
  Serial.println("pos = " + String(pos) + " dir= " + String(dir));

}
