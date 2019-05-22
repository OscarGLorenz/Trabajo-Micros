uint8_t dir = 0;
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
        " push r24\n"
        " push r25\n"
        " push r26\n"
        " push r27\n"
        " push r30\n"
        " push r31\n"

        " ld r24,Z+ \n"
        " ld r25, Z \n"
        
        " ld r16,X \n"
        " tst r16 \n"
        " breq zero \n"
        //sumar 1
        
        "adiw r24,1 \n"

        " jmp end\n"
        "zero: \n"

        "sbiw r24,1 \n"


        "end: \n"

        "sbiw z,1 \n"
        " st Z+,r24 \n"
        " st Z,r25 \n"
        
        " pop r31\n"
        " pop r30\n"
        " pop r27\n"
        " pop r26\n"
        " pop r25\n"
        "pop r24\n"
        " pop r16\n"


        ::"z" (&pos),"x" (&dir):
);

  
  Serial.println("pos = " + String(pos) + " dir= " + String(dir));

}
