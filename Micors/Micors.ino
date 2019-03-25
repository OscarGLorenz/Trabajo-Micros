#define NUM_TOTAL_RAYITAS 5
int bandera1;
int bandera2;
int num_rayitas;
float tiempo;
float tiempo_total;
float rayitas[60];

float cuenta() {
  if (bandera1 == 1 && bandera2 == 0) {
    tiempo = millis();
    if(num_rayitas==1) tiempo_total=millis();
    bandera2 = 1;
  }
  else if (bandera1 == 0 && bandera2 == 1) {
    tiempo = millis() - tiempo;
    bandera2 = 0;
    rayitas[num_rayitas]=tiempo;
    Serial.println(tiempo);
    if(num_rayitas ==NUM_TOTAL_RAYITAS-1){
      tiempo_total=millis()-tiempo_total;
      Serial.println("--------------------------");
      Serial.println(tiempo_total);
      for(int i = 0; i<NUM_TOTAL_RAYITAS; i++){
        Serial.println(rayitas[i]);
      }
      Serial.println("++++++++++++++++++++++++++");
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
