void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, INPUT_PULLUP);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!digitalRead(4)){
    delay(100);
    digitalWrite(2, HIGH);
    delay(29);
    digitalWrite(3, HIGH);
    delay(7);
    digitalWrite(2, LOW);
    delay(25);
    digitalWrite(3, LOW);
    delay(2000);
  }

}
