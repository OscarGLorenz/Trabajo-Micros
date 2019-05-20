uint32_t now;
uint32_t past;


void setup() {
  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:

//delay(10);
  now= 2e6;
  past=1e5;
  
  
  Serial.println(" now= " + String(now)+ " past= " + String(past));

  asm volatile(
  "sub %A0,%A1\n"
  "sbc %B0,%B1\n"
  "sbc %C0,%C1\n"
  "sbc %D0,%D1\n"
  
  :"+r" (now) :"r" (past)
  );
  
  Serial.println(" now= " + String(now)+ " past= " + String(past));


}
