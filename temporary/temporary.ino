#define LED 4
#define S01 3
#define TIEMPO_MAX 20000
float tiempo;
float tiempo_interval;
char traduciendo;
char flanco_actual;
char tipo_flanco;
float rayas[54];
int codigo[24];


char code(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  switch (((a == 0) << 3) | ((b == 0) << 2) | ((c == 0) << 1)  | (d == 0)) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 12:
      return '-';
    case 14:
      return '+';
    case 15:
      return ':';
    default:
      return 'E';
  }
}

float tiempo_raya(int numero) {
  return ((rayas[numero*2+2])-(rayas[numero*2+1]));
}
/*float tiempo_espacio(int numero){
  return ((rayas[numero*2+1])-(rayas[numero*2]));
}*/

void cuenta() {
  tiempo_interval = micros() - tiempo_interval;
  if(tiempo_interval>TIEMPO_MAX) flanco_actual=0;
  rayas[flanco_actual] = micros();
  tiempo_interval = micros();
  flanco_actual++;
}

void traduce() {
  char longitud=0;
  int cuenta_raya=0;
  codigo[0]=0;
  for(cuenta_raya=1;cuenta_raya<24;cuenta_raya++){
    if(longitud==0){
      if(tiempo_raya(cuenta_raya)>(1.2*tiempo_raya(cuenta_raya-1))){
        codigo[cuenta_raya]=1;  
      }
      else codigo[cuenta_raya]=0;
    }
    else{
      if(tiempo_raya(cuenta_raya)<(0.8*tiempo_raya(cuenta_raya-1))){
        codigo[cuenta_raya]=0;  
      }
      else codigo[cuenta_raya]=1;
    }
  }
  for(int i=0;i<19;i+=4){
    Serial.print(code(codigo[i],codigo[i+1], codigo[i+2], codigo[i+3]));
  }
  Serial.println("");
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(S01, INPUT);
  tiempo = 0;
  flanco_actual = 0;
  traduciendo = 0;
  tipo_flanco = 0;
  attachInterrupt(digitalPinToInterrupt(S01), cuenta, CHANGE);
  Serial.begin(9600);
}


void loop() {
  if ((micros() - tiempo) > 1000000) {
    digitalWrite(LED, !digitalRead(LED));
    tiempo = micros();
  }
  if(flanco_actual>=50){
    flanco_actual=0;
    traduce();
  }
}
