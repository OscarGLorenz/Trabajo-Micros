uint32_t times[2] = {1e5, 2e5};
uint32_t dif = 6e5;
uint8_t byte_test;
void setup() {
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  Serial.println(String(times[0])+ " :" + String(times[1]));

  asm volatile(
    "push r16 \n"
    "push r17 \n"
    "push r26 \n"
    "push r27 \n"
    "push r28 \n"
    "push r29 \n"


    "movw  Y,X \n "
    "adiw  Y,4 \n"

    //"lds r16,%1 \n"

    "ld r16, X+ \n"
    "st Y+, r16 \n"
    "ld r16, X+ \n"
    "st Y+, r16 \n"
    "ld r16, X+ \n"
    "st Y+, r16 \n"
    "ld r16, X+ \n"
    "st Y+, r16 \n"







    "pop r29 \n"
    "pop r28 \n"
    "pop r27 \n"
    "pop r26 \n"
    "pop r17 \n"
    "pop r16 \n"

    :"+r" (byte_test):"x" ((uint16_t)times), "r" (dif)
  );

}
