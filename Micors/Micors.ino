#define NUM_TOTAL_RAYITAS 26

int bandera1;
int bandera2;
int num_rayitas;
float tiempo;
float tiempo_total;
float rayitas[NUM_TOTAL_RAYITAS];
float traducidoh[NUM_TOTAL_RAYITAS];



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


void Traduce(){
  float limite=0;
  for(int i=1; i<NUM_TOTAL_RAYITAS;i++){
   limite = limite+rayitas[i];
  }
  limite = limite/(NUM_TOTAL_RAYITAS);
  Serial.println(limite);
  for(int i=1; i<NUM_TOTAL_RAYITAS-1; i++){
    if(rayitas[i]>limite){
      traducidoh[i]=1;
      Serial.print("\n LAAARGO - "+ String(rayitas[i]));
    }
    else{
      traducidoh[i]=0;
      Serial.print("\n CRTO - "+ String(rayitas[i]));
    }
    if(i==NUM_TOTAL_RAYITAS-1) Serial.println("");
  }
  Serial.println("");
  for(int i =1; i<NUM_TOTAL_RAYITAS-4; i=i+4){
    Serial.print(code(traducidoh[i],traducidoh[i+1],traducidoh[i+2],traducidoh[i+3]));
  }
  Serial.println("\n Deduzco que es la tarjeta:");
  Serial.print(":+");
    for(int i =9; i<(NUM_TOTAL_RAYITAS-8); i=i+4){
    Serial.print(code(traducidoh[i],traducidoh[i+1],traducidoh[i+2],traducidoh[i+3]));
  }
  Serial.println("-");
}


float cuenta() {
  if (bandera1 == 1 && bandera2 == 0) {
    tiempo = micros();
    if(num_rayitas==1) tiempo_total=micros();
    bandera2 = 1;
  }
  else if (bandera1 == 0 && bandera2 == 1) {
    tiempo = micros() - tiempo;
    bandera2 = 0;
    rayitas[num_rayitas]=tiempo;
    Serial.println(tiempo);
    if(num_rayitas ==NUM_TOTAL_RAYITAS-2){
      tiempo_total=micros()-tiempo_total;
      Serial.println("-------------Total-------------");
      Serial.println(tiempo_total);
      Serial.println("-------------Imprimiendo vector-------------");
      for(int i = 0; i<NUM_TOTAL_RAYITAS; i++){
        Serial.println(String(i)+" - "+rayitas[i]);
      }
      Serial.println("++++++++++++++++++++++++++");
      Traduce();
    }
    num_rayitas++;
    if (num_rayitas == NUM_TOTAL_RAYITAS) num_rayitas = 0;
  }
}
void setup() {
  pinMode(3, INPUT);
  Serial.begin(9600);
  bandera1 = 0;
  bandera2 = 0;
  num_rayitas=0;
  tiempo_total=0;
  attachInterrupt(digitalPinToInterrupt(3), []() {bandera1 = !bandera1;}, CHANGE);
}

void loop() {
cuenta();
}
