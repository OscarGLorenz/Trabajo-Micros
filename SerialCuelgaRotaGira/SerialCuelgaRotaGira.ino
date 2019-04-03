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

volatile int difference = 0;
volatile int lastPos = 0;

volatile bool pulsDir = 0;
volatile int count = 0;

#define Kfiltr 0.95
void encoder() {
  if (dir) pos++;
  else pos--;
  now = millis();
  dif = now - past;
  filtr = (1-Kfiltr)*dif + Kfiltr *filtr;

  if (count > 0) {
    pulsDir = !pulsDir;
    digitalWrite(DIR,!digitalRead(DIR));   
    count = -1;
  }else if (count > 0) count++;
  
  if ( dif > 2*filtr) {
    if (millis() - lastIn > 300) {
      dir = !dir;  count = 1;
      
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


  digitalWrite(ENA,1);

  while(abs(pos)  < 8) {
    digitalWrite(DIR,sin(5.5*(millis()/1000.0)) >0 );
  }

  
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

 // analogWrite(ENA,abs(duty));
 // digitalWrite(DIR,duty > 0);

  if (millis() - pocoPoco > 5) {
      Serial.print(pos); Serial.print(" "); Serial.print(dir*100-50); Serial.print(" "); Serial.println(pulsDir*100-50);
      pocoPoco = millis();
      difference = pos - lastPos;
      lastPos = pos;

  }
}
