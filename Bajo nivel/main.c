#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include "../charpy/macros.h"
#include "../charpy/time.h"
#include "../charpy/pinout.h"
#include "../charpy/serial.h"

// Variables auxiliares para el encoder
volatile int pos = 0;
volatile bool dir = 0;
volatile long int now = 0;
volatile long int past = 0;
volatile long int dif = 0;
volatile long int lastIn = 0;
volatile float times[5];
volatile float bouncing_time = 500;
// Tiempo inicial de balanceo
unsigned long start_flag = 0;

// Maquina de estados
enum mode_t {ESPERA, CUELGA, ROTA, GIRA, FRENA, EMERGENCIA, CATASTROFE, LOBOTOMIA, CARGA};
enum mode_t mode = ESPERA;

// Parpadeo del ted
bool L4on = false;
unsigned int  L4tiempo = 0 ;

// Interrupción del encoder
ISR(SO4_vect) {

  // Suma o resta dependiendo de la dirección
  if (dir) pos++;
  else pos--;

  // Guarda el tiempo actual y calcula la diferencia
  now = millis();
  dif = now - past;

  // Desplazamiento en array de ultimos tiempos
  for (int j = 0; j < 4; j++) {
    times[j] = times[j + 1];
  }
  times[4] = dif;

  // Mirar si hemos cambiado de dirección
  int aux = (((times[0] + times[1]) * 2.5) - (times[2] + times[3] + times[4]));

  // Limitación de cambios de dirección
  if (now - lastIn > bouncing_time) {
    // Si ha dado positivo el cambio de dirección
    if (aux < 0) {
      // Comprobar si estamos lejos del centro
      if (pos > 40  || pos < -40 ) {
        // Recalcular el tiempo de limitación
        bouncing_time = 3 * (times[2] + times[3] + times[4]);
        // limite inferior al tiempo
        if (bouncing_time < 500) bouncing_time = 500;
      // Si no limite inferior
      } else bouncing_time = 500;

      // Cambia de dirección
      dir = !dir;
      // Toggle M2_dir
      tbi(OUTRUT, M2_di);
      // Guardar tiempo del último cambio
      lastIn = now;
    }
  }

  // Guardar tiempo de ultima Interrupción
  past = now;
}

// Auxiliar para SO5
volatile bool flag = 0;
volatile long int nowInter = 0;
// Interrupción de SO5
ISR(SO5_vect) {
  // Sube una flag con marca de tiempo
  if (rbi(PIN_S05, P_S05)) {
    flag = 1;
    nowInter = millis();
  }
}

void setup() {
  // Pines como output
  DDR_OUTRUT = 0xFF;
  serialBegin(9600);

  // Desactiva Interrupciones
  cli();

  // CHANGE triggers INT3
  sbi(CTRL_INT, SO4_C0);
  cbi(CTRL_INT, SO4_C1);

  // Enable INT3
  sbi(INT_MASK, SO4);

  // Enable PCINT0
  sbi(S05_CTRL, SO5_ENABLE);
  sbi(SO5_MASK, SO5);

  // Activa Interrupciones
  sei();

  start_flag = millis();

}

// Limita la salida por serial
long int pocoPoco = 0;
// Tiempo auxiliar
long int auxTime = 0;

void loop() {
  // Máquina de estados
  switch (mode) {
    // Esperar a persona
    case ESPERA:
      // Si se pulsa el boton han pasado 4 persona
      if (!rbi(PINRUT, SW3)) {
        // Salta al modo carga, guarda tiempo y enciende led
        mode = CARGA;
        auxTime = millis();
        sbi(OUTRUT, L3);
      }
      break;

    // Cargar carne
    case CARGA :
      // Espera 10 segundos
      if (millis() - auxTime > 10000 ) {
        mode = CUELGA;
        // Apaga led y enciende motor
        sbi(OUTRUT, M2_en);
        cbi(OUTRUT, L3);
      }
      break;

    // Fase de arranque
    case CUELGA:

      // Cuando el ángulo sea suficiente salta al siguiente modo
      if (pos < 10) {
        // Durante 1 segundo hacia un lado
        if (millis() - start_flag < 1000) {
          cbi(OUTRUT, M2_di);
        }
        // Ahora oscila cada 600 ms
        else if (millis() % 1200 > 1200 / 2)
          sbi(OUTRUT, M2_di);
        else
          cbi(OUTRUT, M2_di);
      } else {
        // Salta de modo y guarda tiempo
        mode = ROTA;
        lastIn = now;
      }
      break;

    case ROTA:
      // Si llevamos dos segundos sin cambiar de dirección pasa a girar
      if (millis() - lastIn > 2000) {
        mode = GIRA;
      }
      break;
    case GIRA:
      // Ahora 10 segundos de vueltas sin parar
      if (millis() - lastIn > 10000) {
        // Ahora frena a contramarcha
        mode = FRENA;
        dir = !dir;
        tbi(OUTRUT, M2_di); // Toggle M2_dir
        auxTime = millis();
      }
      break;

    case FRENA:
      // 12 segundos para frenar
      if (millis() - auxTime > 12000) {
        mode = ESPERA;
        // Ahora apaga motor
        cbi(OUTRUT, M2_en);
        cbi(OUTRUT, M2_di);
      }
      break;

    case CATASTROFE:
      // 5 segundos frenando
      if (millis() - auxTime > 5000) {
        mode = EMERGENCIA;
        auxTime = millis();
      }

      break;
    case EMERGENCIA:
      // Otros 5 segundos frenando
      if (millis() - auxTime > 5000) {
        mode = LOBOTOMIA;
      }

      break;
    case LOBOTOMIA:
      // Apaga el motor
      cbi(OUTRUT, M2_en);
      cbi(OUTRUT, M2_di);
      break;

  }

  // Atiende el Paso por S05, con antirrebote
  if ( millis() - nowInter > 1 && flag) {
    if (rbi(PINB, PB0)) {
      pos = +6;
    }
    flag = 0;
  }

  // Boton de EMERGENCIA
  if (!rbi(PINRUT, SW1)) {
    // Si estamos en modo rotar frenar 5 segundos
    if (mode == ROTA) {
      mode = EMERGENCIA;
      dir = !dir;
      tbi(OUTRUT, M2_di);
      auxTime = millis();
    // Si cuelga o espera, simplemente apaga motor
    } else if (mode == CUELGA || mode == ESPERA) {
      mode = LOBOTOMIA;
    // Si estamos en frena pasa a emergencia
    } else if (mode == FRENA) mode = EMERGENCIA;
    // Si estamos en gira, frena 10 segundos
    else if (mode == GIRA) {
      mode = CATASTROFE;
      dir = !dir;
      tbi(OUTRUT, M2_di); // Toggle M2_dir
      auxTime = millis();

    }
  }

  // Si la atracción funciona parpadea 0.5s cada 10s
  if (mode == CUELGA || mode == ROTA || mode == GIRA || mode == FRENA) {
    if (L4on) {
      if (millis() - L4tiempo > 500) {
        L4on = false;
        cbi(OUTRUT, L4);
        L4tiempo = millis();
      }
    } else {
      if (millis() - L4tiempo > 10000) {
        L4on = true;
        sbi(OUTRUT, L4);
        L4tiempo = millis();

      }
    }
  }
  // Si la atracción esta en emergencia parpadea 0.2s cada 1s
  else if (mode == LOBOTOMIA || mode == CATASTROFE || mode == EMERGENCIA ) {
    if (L4on) {
      if (millis() - L4tiempo > 200) {
        L4on = false;
        cbi(OUTRUT, L4);
        L4tiempo = millis();

      }
    } else {
      if (millis() - L4tiempo > 1000) {
        L4on = true;
        sbi(OUTRUT, L4);
        L4tiempo = millis();

      }
    }
  } else {
    cbi(OUTRUT, L4);
  }

  // Mostrar por serie
  if (millis() - pocoPoco > 5) {
    pocoPoco = millis();
    serialPrintFloat(dif);
    serialWrite(' ');
    serialPrintFloat(pos);
    serialWrite(' ');
    serialPrintFloat(bouncing_time);
    serialWrite(' ');
    serialPrintFloat((dir) ? 50.0 : -50.0);
    serialWrite(' ');
    serialPrintFloat(((int) mode) * 100);
    serialWrite('\n');
  }

}

int main() {
  initTime();
  setup();

  for (;;)
    loop();

  return 0;
}
