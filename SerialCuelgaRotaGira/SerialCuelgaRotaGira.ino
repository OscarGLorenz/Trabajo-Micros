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
  Serial.begin(115200);
  Serial.setTimeout(10);

  attachInterrupt(digitalPinToInterrupt(ENC),encoder,CHANGE);
  attachInterrupt(digitalPinToInterrupt(SEN),sense,RISING);
  pinMode(DIR,OUTPUT);
  pinMode(ENA,OUTPUT);
}


int duty = 0;
long int pocoPoco = 0;
void loop() {
  if (Serial.available() > 0) {
    duty = Serial.parseInt(); Serial.parseInt();
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
