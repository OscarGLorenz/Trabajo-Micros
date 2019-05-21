volatile uint32_t times[5] = {1e5, 2e5, 3e5, 4e5, 5e5};
volatile uint32_t now = 6e5;

void setup() {
  Serial.begin(115200);
    delay(500);
  Serial.println(String(times[0])+ " :" + String(times[1])+ " :" + String(times[2])+ " :" + String(times[3])+ " :" + String(times[4])+ " :" + String(now));

}

void loop() {
  // put your main code here, to run repeatedly:
    delay(500);

  asm volatile(
    "push r29         \n"
    "push r28         \n"
    "movw  Y,X        \n"
    "adiw  Y,4        \n"
    "ldi r18,4        \n"
    "loop2:           \n"
    "   ldi r17,4     \n"
    "   loop1:        \n"
    "     ld r16, Y+  \n"
    "     st X+, r16  \n"
    "     dec r17     \n"
    "     tst r17     \n"
    "     brne loop1  \n"
    "   dec r18       \n"
    "   tst r18       \n"
    "   brne loop2    \n"
    "ldi r17,4        \n"
    "loop3:           \n"
    "   ld r16, Z+    \n"
    "   st X+, r16    \n"
    "   dec r17       \n"
    "   tst r17       \n"
    "   brne loop3    \n"
    "pop r28         \n"
    "pop r29         \n"
    ::"x" ((uint16_t)times), "z" ((uint16_t)&now) : "r16", "r17", "r18"
  );
    delay(500);

    Serial.println(String(times[0])+ " :" + String(times[1])+ " :" + String(times[2])+ " :" + String(times[3])+ " :" + String(times[4])+ " :" + String(now));


}
