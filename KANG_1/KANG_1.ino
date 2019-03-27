#define ENA 5
#define DIR 4
#define ENC 2
#define SEN 3

volatile int pos = 0;
volatile bool dir = 0;
volatile long int now = 0;
volatile long int past = 0;
volatile float filtr = 0;
volatile long int dif = 0;
volatile long int lastIn = 0;

#define Kfiltr 0.95
void encoder() {
  if (dir) pos++;
  else pos--;
  now = millis();
  dif = now - past;
  filtr = (1-Kfiltr)*dif + Kfiltr *filtr;

  if ( dif > 2*filtr) {
    if (millis() - lastIn > 300) {
      dir = !dir;    
      lastIn = millis();
    }
  }
  
  past = now;
}

volatile bool flag = 0;
volatile long int nowInter = 0;
void sense() {
    flag = 1;
    nowInter = millis();
    //pos = 0;
}

void setup() {
  Serial.begin(1000000);
  Serial.setTimeout(10);

  attachInterrupt(digitalPinToInterrupt(ENC),encoder,CHANGE);
  attachInterrupt(digitalPinToInterrupt(SEN),sense,RISING);
  pinMode(DIR,OUTPUT);
  pinMode(ENA,OUTPUT);
}


double phi = 0; double omega = 1;
long int pocoPoco = 0;
void loop() {
  int duty = 255*cbrt(sin(omega*(millis()/1000.0)+phi));
  if (Serial.available() > 0) {
    phi = Serial.parseFloat(); omega = Serial.parseFloat(); Serial.parseFloat();
  }
  if ( millis() - nowInter > 1 && flag) {
    if (digitalRead(SEN))  {
      pos = 0;
    }
    flag = 0;
  }
  analogWrite(ENA,abs(duty));
  digitalWrite(DIR,duty > 0);

  if (millis() - pocoPoco > 5) {
      Serial.println(pos);
      pocoPoco = millis();
  }
}
