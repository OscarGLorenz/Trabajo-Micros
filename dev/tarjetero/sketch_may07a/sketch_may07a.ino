#include <math.h>
#define NUM_TOTAL_RAYITAS 26
#define LRG 0
#define CRT 1
#define SENSOR 3
#define rbi(data,bit) ((data & (bit << i)) != 0)

int bandera1;
int bandera2;
int num_rayitas;
float tiempo;
float tiempo_total;
float rayitas[NUM_TOTAL_RAYITAS];
char traducidoh[NUM_TOTAL_RAYITAS];
uint16_t aiuda;
uint16_t a[] = {0x0123, 0x0456, 0x0789, 0x0097, 0x0531, 0x0642};
int n = 6;
int muchobueno = 0;
int luz;




char code(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  switch (((a) << 3) | ((b) << 2) | ((c) << 1)  | (d)) {
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

int whoMax(float *c) {
  float max = c[0];
  int who = 0;
  for (int i = 1; i < n; i++) {
    if (c[i] > max) {
      who = i;
      max = c[i];
    }
  }
  return who;
}
float sumData(uint16_t c) {
  float howmuch = 0;
  for (int i = 0; i < 16; i++) {
    howmuch += rbi(c, i);
  }
  return howmuch;
}

void aiudado() {
  aiuda = 0;
  for (int i = 20; i > 8; i--) {
    if (traducidoh[i]) {
      aiuda  |= (1 << 20 - i);
    }
  }
  Serial.print("Aiudame: ");
  Serial.println(aiuda, HEX);
  float jaccard[n];
  for (int j = 0; j < n; j++) {
    jaccard[j] = sumData( a[j] & aiuda) / sumData( a[j] | aiuda);
  }
  Serial.println(jaccard[whoMax(jaccard)]);
  Serial.print("Jaccard TODOPODERSO dice... :");
  if ((jaccard[whoMax(jaccard)] > 0.75) || (((aiuda && 0x0FF) == 0x0FF))) { //Si hay 1 letra mayor que D, descartamos mucho malo
    Serial.println(a[whoMax(jaccard)], HEX);
    digitalWrite(4, HIGH);
    luz=millis();
  } else {
    Serial.println("Mucho malo idiota");
  }
}

void Traduce() {
  float limite = 0;
  for (int i = 1; i < NUM_TOTAL_RAYITAS; i++) {
    limite = limite + rayitas[i];
  }
  limite = limite / (NUM_TOTAL_RAYITAS);
  for (int i = 1; i < NUM_TOTAL_RAYITAS - 1; i++) {
    if (rayitas[i] > limite) {
      traducidoh[i] = LRG;
    }
    else {
      traducidoh[i] = CRT;
    }
  }
  //-------------------------------
  Serial.println("");
  Serial.print("Imprimiendo Traducidoh:");
  for (int i = 1; i < NUM_TOTAL_RAYITAS - 1; i++) {
    int aux = traducidoh[i];
    Serial.print(aux);
    if (!(i % 4)) Serial.print(" ");
  }
  Serial.println("");
  //-------------------------------
  Serial.println("");
  for (int i = 1; i < NUM_TOTAL_RAYITAS - 4; i = i + 4) {
    Serial.print(code(traducidoh[i], traducidoh[i + 1], traducidoh[i + 2], traducidoh[i + 3]));
  }
  Serial.println("\n Deduzco que es la tarjeta:");
  Serial.print(":+");
  for (int i = 9; i < (NUM_TOTAL_RAYITAS - 8); i = i + 4) {
    Serial.print(code(traducidoh[i], traducidoh[i + 1], traducidoh[i + 2], traducidoh[i + 3]));
  }
  Serial.println("-");
  aiudado();
  //compara();
}


float cuenta() {
  if (bandera1 == 1 && bandera2 == 0) {
    tiempo = micros();
    if (num_rayitas == 1) tiempo_total = micros();
    bandera2 = 1;
  }
  else if (bandera1 == 0 && bandera2 == 1) {
    tiempo = micros() - tiempo;
    bandera2 = 0;
    rayitas[num_rayitas] = tiempo;
    if (num_rayitas == NUM_TOTAL_RAYITAS - 2) {
      tiempo_total = micros() - tiempo_total;
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
  num_rayitas = 0;
  tiempo_total = 0;
  luz = millis();
  attachInterrupt(digitalPinToInterrupt(SENSOR), []() {
    bandera1 = !bandera1;
  }, CHANGE);
}



void loop() {
  cuenta();
  if((millis()-luz)>1000){
    digitalWrite(4, LOW);    
  }
}
