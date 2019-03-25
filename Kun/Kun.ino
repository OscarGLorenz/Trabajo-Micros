#define INTERRUPT_PIN 21
#define MAX 26

float timer;
float Tiempos[MAX]; //vector
int counter;
float largo[MAX - 2];


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

void ancho() {
  float lim = 0;
  for (int i = 1; i < MAX - 1; i++) { //Media
    lim += Tiempos[i];
  }
  lim = lim / (MAX - 2);
  for (int i = 1; i < MAX - 1; i++) { //Diferenciar largo corto
    largo[i - 1] = (Tiempos[i] > lim);
  }
  Serial.print("-----------------CODIGO-----------------");
  for (int i = 0; i < MAX - 2; i += 4) { //Imprimir codigo
    Serial.print(code(largo[i], largo[i + 1], largo[i + 2], largo[i + 3]));
  }
  Serial.println("");
}

void Kun() {
  if (!digitalRead(INTERRUPT_PIN))
    timer = micros();
  else{
    timer = micros() - timer;
    Serial.println(timer);
  }
  Tiempos[counter] = timer;
  counter++;
  
}

void setup() {
  Serial.begin(9600);
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), Kun, CHANGE);
  counter = 0;
  timer = 0;

}

void loop() {
  if (counter == MAX) {
    ancho();
    counter = 0;
  }
}
