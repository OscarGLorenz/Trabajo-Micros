
float tiempos[51];
float tiempo4[6];
int longitud[24];
int actual=0;

void cuenta(){
  tiempos[actual] = micros();
  actual++;
  if(actual==50) actual=0;
}

char code(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  switch(((a==0) << 3) | ((b==0) << 2) | ((c==0) << 1)  | (d==0)) {
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

float tiempo_raya(int num){
  return (tiempos[num*2+1]-tiempos[num*2]);
}
float tiempo_espacio(int num){
  return (tiempos[num*2+2]-tiempos[num*2+1]);
}
float tiempo_grupo(int num){
  float total = 0;
  for(int i = num; i<(num+4);i++){
    total += tiempo_raya(i)+tiempo_espacio(i);
  }
  return total;
}

void imprime(){
  for(int i=0; i<25; i++)
  Serial.println(String(tiempo_raya(i))+ " - " + String(longitud[i]));
}
void traduce(){
  float limite;
  imprime();
  for(int i =0; i<6; i++){
    tiempo4[i] = tiempo_grupo(i+1);
  }
  limite = ((tiempo_raya(24)+tiempo_raya(23)+tiempo_raya(22)+tiempo_raya(21))/tiempo4[5])/4.0;
  for(int i =0; i<6;i++){
    for(int j=1;j<5;j++){
      if((tiempo_raya(4*i+j)/tiempo4[i])>limite) longitud[4*i+j]=1;
      else longitud[4*i+j]=0;
    }
  }
  Serial.print(":+");
    for(int i =9; i<18; i=i+4){
    Serial.print(code(longitud[i],longitud[i+1],longitud[i+2],longitud[i+3]));
  }
  Serial.println("-");
}


void setup() {
  pinMode(3, INPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Serial.begin(9600);
  //attachInterrupt(digitalPinToInterrupt(3), []() {bandera1 = !bandera1;}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), cuenta, CHANGE);
}

void loop() {
  if(actual==0) traduce();
}
