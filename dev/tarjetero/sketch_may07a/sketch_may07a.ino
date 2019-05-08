//#include <math.h>
#define NUM_TOTAL_RAYITAS 26		//NÚMERO DE RAYAS QUE NOS ENCONTRAMOS POR TARJETA
#define LRG 0						//DEFINIMOS RAYAS LARGAS CON CÓDIGO 0
#define CRT 1						//DEFINIMOS RAYAS CORTAS CON CÓDIGO 1
#define SENSOR 3					//PIN DEL SENSOR EN ARDUINO (PORQUE TIENE INTERRUPCIÓN)
#define rbi(data,bit) ((data & (bit << i)) != 0)

int bandera1;  														//FLANCO DE INICIO DE RAYA DETECTADO
int bandera2;														//FLANCO DE FIN DE RAYA DETECTADO
int num_rayitas;													//RAYA ACTUAL EN LA LECTURA
float tiempo;														//VARIABLE AUXILIAR PARA GUARDAR LA DURACIÓN DE LA RAYA ACTUAL
float tiempo_total;													//TIEMPO TOTAL DE LA TARJETA, USADO PARA DISTINGUIR RAYAS LARGAS Y CORTAS
float rayitas[NUM_TOTAL_RAYITAS];									//VECTOR DONDE SE GUARDAN TIEMPOS
char traducidoh[NUM_TOTAL_RAYITAS];									//VECTOR BINARIO DONDE DISTINGUIMOS ENTRE RAYAS LARGAS (0) Y CORTAS (1)
uint16_t aiuda;														//UTILIZADO PARA ALGORITMO DE COMPARACIÓN Y ESTIMACIÓN. SE SACA A PARTIR DEL VECTOR traducidoh
uint16_t a[] = {0x0123, 0x0456, 0x0789, 0x0097, 0x0531, 0x0642};	//POSIBLES COMBINACIONES DE TARJETAS
int n = 6;															//NÚMERO TOTAL DE TARJETAS POSIBLES. AUXILIAR.
int muchobueno = 0;													//DETECTADA UNA TARJETA VÁLIDA
int luz;															//VARIABLE PARA ENCENDER EL LED DURANTE 1 SEGUNDO SIN BLOQUEOS

//FUNCIÓN AUXILIAR, UTILIZADA PARA IMPRIMIR, NO NECESARIA PARA EL FUNCIONAMIENTO
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

int whoMax(float *c) {		//DEVUELVE MEJOR COINCIDENCIA (MÁS PESO) SEGÚN EL ÍNDICE CALCULADO
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
float sumData(uint16_t c) {		//NÚMERO DE '1' EN EL DATO C. USADO PARA CALCULAR COINCIDENCIAS
  float howmuch = 0;
  for (int i = 0; i < 16; i++) {
    howmuch += rbi(c, i);
  }
  return howmuch;
}
//TRADUCCIÓN DE traducidoh A aiuda. (ES NECESARIA LA CONVERSIÓN DE UN VECTOR A UNA VARIABLE INT PARA QUE FUNCIONE LA FÓRMULA DE JACCARD (TANIMOTO)). SE REALIZA LA ESTIMACIÓN.
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
  if ((jaccard[whoMax(jaccard)] > 0.75) || (((aiuda && 0x0FF) == 0x0FF))) { //Si hay 1 letra mayor que D, descartamos mucho malo. Si no, todo bien.
    Serial.println(a[whoMax(jaccard)], HEX);
    digitalWrite(4, HIGH);
    luz=millis();
  } else {																	//Descartamos
    Serial.println("Mucho malo idiota");
  }
}

//PASAMOS DE UN VECTOR DE TIEMPOS A UN VECTOR QUE NOS DIGA QUÉ RAYAS SON CORTAS Y QUÉ RAYAS SON LARGAS
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

//VAMOS GUARDANDO LA DURACIÓN DE LAS RAYAS. ES POSIBLE QUE HAYA QUE PASAR A LA INTERRUPCIÓN EL ALMACENAMIENTO DE TIEMPOS PARA MEJORAR LA PRECISIÓN.
float cuenta() {
  if (bandera1 == 1 && bandera2 == 0) { 		//HA ENTRADO UNA NUEVA RAYA
    tiempo = micros();
    if (num_rayitas == 1) tiempo_total = micros();
    bandera2 = 1;
    if(num_rayitas>2 && rayitas[num_rayitas-1]>30000) num_rayitas = 0;
  }
  else if (bandera1 == 0 && bandera2 == 1) {	//SALIMOS DE LA RAYA ACTUAL
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
  /*
  if(num_rayitas>=1 && 
  }
  */
}

void setup() {
	
  pinMode(SENSOR, INPUT);
  Serial.begin(9600);
  bandera1 = 0;
  bandera2 = 0;
  num_rayitas = 0;
  tiempo_total = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), []() {bandera1 = !bandera1;}, CHANGE);
}



void loop() {
  //vamos actualizando datos del código de la tarjeta según podamos
  cuenta();
  //encendemos durante 1 segundo el LED cuando detectamos una tarjeta válida
  if((millis()-luz)>1000){
    digitalWrite(4, LOW);    
  }
}
