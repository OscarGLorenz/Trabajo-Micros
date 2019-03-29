#define S02 2
#define S03 3

uint8_t t02;
uint8_t t03;
uint8_t dly;
uint8_t ancho02;
uint8_t ancho03;
uint8_t total;
float rel;

void sensor_S02() {
  if (digitalRead(S02)) {
    t02 = millis();
  }
  else {
    ancho02 = millis() - t02;
    Serial.println("Moneda: ancho02 ancho03 dly total");
    Serial.print(String(ancho02) + "   ");
  }
}

void sensor_S03() {
  if (digitalRead(S03)) {
    t03 = millis();
    dly = millis()- t02;
  }
  else {
    ancho03 = millis() - t03;
    total = millis() - t02;
    Serial.print(String(ancho03) + "   ");
    Serial.print(String(dly) + "   ");
    Serial.print(String(total) + "   ");
    Serial.println("Medida relativa: ");
    rel = float(ancho03)/dly;
    Serial.print(String(rel) + "   ");
  }
}


void setup() {
  pinMode(S02, INPUT);
  pinMode(S03, INPUT);
  attachInterrupt(digitalPinToInterrupt(S02), sensor_S02, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S03), sensor_S03, CHANGE);
  Serial.begin(9600);
}

void loop() {
}
