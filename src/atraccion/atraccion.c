/*************************************************************
*
* FICHERO: atraccion.c
*
* DESCRIPCIÓN :
* 	Contiene la funcionalidad de la atracción. Implementada como
*	máquina de estados
*
* FUNCIONES :
*
* AUTORES:
*
* 	Miguel Fernández Cortizas
*
*   Óscar García Lorenz
*
*
*************************************************************/

#include "time.h"	// Para utilizar millis()
#include "macros.h"	// Macros auxiliares para manejo de puertos
#include "pinout.h" // Descripción de interconexiones
#include "serial.h" // Puerto serie

#include "atraccion.h"	// Cabecera del módulo atracción
#include <stdint.h>		// Tipos estandar de enteros

//#define SERIAL_DEBUG // Salida de datos por el puerto serie

// Guardar callback atraccion finalizada
static void (*finalizado) ();
void atraccionSetCallbackFinalizado(void(*f)()) {
	finalizado = f;
}

// Guardar callback de atraccion en emergencia
static void (*emergencia) ();
void atraccionSetCallbackEmergencia(void(*f)()) {
	emergencia = f;
}

// Variables auxiliares para el encoder
static volatile int16_t pos = 0;			// posición
static volatile uint8_t dir = 0;			// dirección
static volatile int32_t now = 0;			// tiempo actual
static volatile int32_t past = 0;			// último tiempo
static volatile int32_t dif = 0;			// diferencia entre tiempos
static volatile int32_t lastIn = 0;			// último cambio de dirección
static volatile int32_t times[5];			// últimos 5 valores de dif
static volatile int16_t bouncing_time=520;	// constante para limitar los cambios de dirección

// Máquina de estados
enum mode_t {ESPERA,CUELGA,ROTA,GIRA,FRENA,EMERGENCIA,CATASTROFE,LOBOTOMIA,CARGA};
static enum mode_t mode = ESPERA;

static long int auxTime = 0; // tiempo auxiliar para controlar cambios de estados

// Esta función se llama desde el main cuando se quiere iniciar la atracción
void atraccionIniciar() {
	// Solo si estamos en el estado ESPERA
	if (mode == ESPERA) {
		// Pasamos al estado carga
		mode = CARGA;
		// Y nos guardamos el tiempo, para esperar 10 segundos después
		auxTime = millis();
		// Encendemos el led verde
		sbi(OUTRUT,L3);
	}
}

// Interrupción del encoder
ISR(SO4_vect){

	// En función de la dirección se incrementa o decrementa la posición
	if (dir) pos++;
	else pos--;

	// Esta rutina es equivalente a lo de arriba
/*
        asm volatile(
        " push r16\n"
        " push r24\n"
        " push r25\n"
        " push r26\n"
        " push r27\n"
        " push r30\n"
        " push r31\n"

		// Cargamos pos
        " ld r24,Z+ \n"
        " ld r25, Z \n"

        // Cargamos dir
        " ld r16,X \n"
        " tst r16 \n"
        " breq zero \n"

        // sumar 1
        "adiw r24,1 \n"

        " jmp end\n"
        "zero: \n"

		// restar 1
        "sbiw r24,1 \n"


        "end: \n"

		// Guardamos pos
        "sbiw z,1 \n"
        "st Z+,r24 \n"
        "st Z,r25 \n"

        " pop r31\n"
        " pop r30\n"
        " pop r27\n"
        " pop r26\n"
        " pop r25\n"
        " pop r24\n"
        " pop r16\n"


        ::"z" (&pos),"x" (&dir):
);
// end asm 1
*/
	// Nos guardamos el valor de tiempo actual
	now = millis();
	// Hacemos una copia de now
	volatile uint32_t ahora = now;
	// Calculamos la diferencia de tiempos
	dif = now - past;

	// Diferencia entre now y past
/*      asm volatile(
        "push r26 \n"
        "push r27 \n"
        "push r30 \n"
        "push r31 \n"
        "push r16 \n"
        "push r17 \n"

		// Ejecuta la resta, utilizando pocos registros
        "ld r16,X \n"
        "ld r17,Z+ \n"
        "sub r16,r17\n"
        "st X+,r16\n"

        "ld r16,X \n"
        "ld r17,Z+ \n"
        "sbc r16,r17\n"
        "st X+,r16\n"

        "ld r16,X \n"
        "ld r17,Z+ \n"
        "sbc r16,r17\n"
        "st X+,r16\n"

        "ld r16,X \n"
        "ld r17,Z+ \n"
        "sbc r16,r17\n"
        "st X+,r16\n"

        "pop r17 \n"
        "pop r16 \n"
        "pop r31 \n"
        "pop r30 \n"
        "pop r27 \n"
        "pop r26 \n"

        ::"x" (&now),"z" (&past)
); dif = now;*/

	// Devolvemos a past el valor de ahora
    past = ahora;


	// Hacemos un desplazamiento en times e introducimos al final dif
	for (int j=0;j<4;j++)
		times[j]=times[j+1];
	times[4] = dif;


/*	// Desplazamiento de times y dif
	asm volatile(
        "push r31         \n"
        "push r30         \n"
        "push r29         \n"
        "push r28         \n"
        "push r27         \n"
        "push r26         \n"
        "push r16         \n"
        "push r17         \n"
        "push r18         \n"
        "movw  Y,X        \n" // Copia los dos punteros
        "adiw  Y,4        \n" // Avanza al siguiente elemento de array
        "ldi r18,4        \n" // Contador a i=4
                              // times[n] = times[n+1]
        "loop2:           \n"
        "   ldi r17,4     \n" // Contador a j=4
        "   loop1:        \n"
        "     ld r16, Y+  \n" // Se va moviendo byte a byte
        "     st X+, r16  \n"
        "     dec r17     \n" // j--
        "     tst r17     \n" // Si j=0 finalizar
        "     brne loop1  \n"
        "   dec r18       \n" // i--
        "   tst r18       \n" // Si i=0 finalizar
        "   brne loop2    \n"
        "ldi r17,4        \n" // k=4
        "loop3:           \n"
        "   ld r16, Z+    \n" // Mover dif a times[4]
        "   st X+, r16    \n"
        "   dec r17       \n" // k--
        "   tst r17       \n" // Si k=0 finalizar
        "   brne loop3    \n"
        "pop r18          \n"
        "pop r17          \n"
        "pop r16          \n"
        "pop r26         \n"
        "pop r27         \n"
        "pop r28         \n"
        "pop r29         \n"
        "pop r30         \n"
        "pop r31         \n"
        ::"x" ((uint16_t)times), "z" ((uint16_t)&dif)
);*/
        volatile int32_t aux;

      //aux = (  ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )  - (times[2]+times[3]+times[4])   ;


        asm volatile(
        "push r31         \n"
        "push r30         \n"
        "push r27         \n"
        "push r26         \n"
        "push r16         \n"
        "push r17         \n"
        "push r18         \n"
        "push r19         \n"
        "push r20         \n"

        "clr r17      \n"
        "clr r18      \n"
        "clr r19      \n"
        "clr r20      \n"

        // times[0]
        "ld r16, X+   \n"
        "add r17, r16 \n"
        "ld r16, X+   \n"
        "adc r18, r16 \n"
        "ld r16, X+   \n"
        "adc r19, r16 \n"
        "ld r16, X+   \n"
        "adc r20, r16 \n"

        // times[0] + times[1]
        "ld r16, X+   \n"
        "add r17, r16 \n"
        "ld r16, X+   \n"
        "adc r18, r16 \n"
        "ld r16, X+   \n"
        "adc r19, r16 \n"
        "ld r16, X+   \n"
        "adc r20, r16 \n"

        // (times[0] + times[1]) << 2
        "rol r17  \n"
        "rol r18  \n"
        "rol r19  \n"
        "rol r20  \n"
        "rol r17  \n"
        "rol r18  \n"
        "rol r19  \n"
        "rol r20  \n"

        // (times[0] + times[1]) << 2 + (times[0] + times[1])
        "sbiw X, 8    \n"
        "ld r16, X+   \n"
        "add r17, r16 \n"
        "ld r16, X+   \n"
        "adc r18, r16 \n"
        "ld r16, X+   \n"
        "adc r19, r16 \n"
        "ld r16, X+   \n"
        "adc r20, r16 \n"
        "ld r16, X+   \n"
        "add r17, r16 \n"
        "ld r16, X+   \n"
        "adc r18, r16 \n"
        "ld r16, X+   \n"
        "adc r19, r16 \n"
        "ld r16, X+   \n"
        "adc r20, r16 \n"
//
//    // ((times[0] + times[1]) << 2 + (times[0] + times[1])) >> 1 = ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )
        "lsr r17  \n"
        "lsr r18  \n"
        "lsr r19  \n"
        "lsr r20  \n"

        // -= (times[2]+times[3]+times[4])
        "ld r16, X+      \n"
        "sub r17, r16    \n"
        "ld r16, X+      \n"
        "sbc r18, r16    \n"
        "ld r16, X+      \n"
        "sbc r19, r16    \n"
        "ld r16, X+      \n"
        "sbc r20, r16    \n"

        "ld r16, X+      \n"
        "sub r17, r16    \n"
        "ld r16, X+      \n"
        "sbc r18, r16    \n"
        "ld r16, X+      \n"
        "sbc r19, r16    \n"
        "ld r16, X+      \n"
        "sbc r20, r16    \n"

        "ld r16, X+      \n"
        "sub r17, r16    \n"
        "ld r16, X+      \n"
        "sbc r18, r16    \n"
        "ld r16, X+      \n"
        "sbc r19, r16    \n"
        "ld r16, X+      \n"
        "sbc r20, r16    \n"

        "st Z+,r17       \n"
        "st Z+,r18       \n"
        "st Z+,r19       \n"
        "st Z+,r20       \n"

        "pop r20         \n"
        "pop r19         \n"
        "pop r18         \n"
        "pop r17         \n"
        "pop r16         \n"
        "pop r26         \n"
        "pop r27         \n"
        "pop r30         \n"
        "pop r31         \n"
        ::"x" ((uint16_t)times), "z" ((uint16_t)&aux)
);



    // Si el último cambio de dirección fue hace más tiempo que bouncing time
	if (now - lastIn > bouncing_time){
		// Y aux es negativo
		if (aux < 0){

/*

			 if (pos > 40  || pos < -40 ){
			 	bouncing_time = 3 *(times[2]+times[3]+times[4]);
				if (bouncing_time < 500) bouncing_time = 500;
			 } else bouncing_time = 500;

*/

			// Cambiamos de dirección el motor
			dir = !dir;
			// Toggle M2_dir
			tbi(OUTRUT,M2_di);
			// Nos guardamos el valor del tiempo
			lastIn = now;
		}
	}


}

// Sensor inferior SO5
static volatile bool flag = 0; // Esta flag se activa con la interrupción
static volatile long int nowInter = 0;	// Aqui se guarda el tiempo en la interrupción
ISR(SO5_vect) {
	// Nos aseguramos que el sensor esté a nivel alto
	if (rbi(PIN_S05,P_S05)) {
		flag = 1;	// Levantamos la flag
		nowInter = millis(); // Guardamos el tiempo
	}
}

// Led 4
static bool L4on = true;	// Estado del led
static unsigned int L4tiempo = 0 ;	// Variable para controlar el parpadeo
// Esta función se llama periodicamente con un tiempo de on y de off
void parpadearL4(unsigned int on, unsigned int off) {
	// En función del estado
	if (L4on){
		// Si ha pasado tiempo suficiente apagamos el led
		if (millis()-L4tiempo > on){
			L4on = false;
			cbi(OUTRUT,L4);
			L4tiempo = millis();
		}
	} else{
		// Si ha pasado tiempo suficiente encendemos el led
		if (millis()-L4tiempo > off){
			L4on = true;
			sbi(OUTRUT,L4);
			L4tiempo = millis();
		}
	}
}

// Esta función es llamada con el main y configura los periféricos necesarios.
void atraccionSetup() {
	// Desactivar interrupciones.
	cli();

	// Configura SO4 como interrupción por flanco de subida y bajada.
	sbi(CTRL_INT, SO4_C0);
	cbi(CTRL_INT, SO4_C1);

	// Activa S04 como interrupción.
	sbi(INT_MASK, SO4);

	// Activa S05 como PCINT. La máscara y la activación.
	sbi(S05_CTRL, SO5_ENABLE);
	sbi(SO5_MASK, SO5);

	// Activar interrupciones
	sei();
}

// Función principal de la atracción
void atraccionLoop() {
	// Máquina de estados
	switch(mode) {
		case ESPERA:
		break;

		case CARGA :
		// Espera 10s para cambiar al modo cuelfa
		if (millis() - auxTime > 100 ){
			mode = CUELGA;
			// Encendemos motor y apagamos luz.
			sbi(OUTRUT,M2_en);
			cbi(OUTRUT,L3);
			// Nos guardamos el tiempo
			auxTime= millis();
		}
		break;

		case CUELGA:
		// Cuando la posición sea mayor que 10 pasamos a ROTA
		if(pos < 10) {
			// Si seguimos en cuelga, movemos el motor en una dirección 1s
			// y luego hacemos oscilar al motor con un periodo de 0.6s
			if (millis() - auxTime < 1000)
				cbi(OUTRUT,M2_di);
			else if (millis()%1200 > 1200/2)
				sbi(OUTRUT,M2_di);
			else
				cbi(OUTRUT,M2_di);
		} else {
			// Se pasa a modo rota, nos guardamos el tiempo
			mode = ROTA;
			lastIn = now;
            auxTime= millis();
        }
		break;

		case ROTA:
		// Si llevamos 2s sin cambiar de dirección pasa a gira
		if (millis() - lastIn > 2000) {
			mode = GIRA;
		}
		// Si llevamos 100s aqui, pasamos a frena
		if (millis() - auxTime > 100000 ) {
                mode = FRENA;
		}
		break;

		case GIRA:
		// Si llevamos 10s aqui pasamosa frena
		if (millis() - lastIn > 10000) {
			mode = FRENA;
			// Cambiamos de dirección para frenar
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			// Nos guardamos el tiempo
			auxTime = millis();
		}
		break;

		case FRENA:
		// Tras 20s en este estado, pasamos a esperar
		if (millis() - auxTime > 20000) {
			mode = ESPERA;
			// Apagamos el motor
			cbi(OUTRUT,M2_en);
			cbi(OUTRUT,M2_di);
			// Ejecutamos el callback de finalizado para que el main
			// pueda volver a iniciar la atracción
			finalizado();
		}
		break;

		case CATASTROFE:
		// Tras 5s en este modo pasamos a emergencia
		if (millis() - auxTime > 5000) {
			mode = EMERGENCIA;
			auxTime = millis();
		}
		break;

		// Tras 5s en este modo pasamos a lobotomia
		case EMERGENCIA:
		if (millis() - auxTime > 5000) {
			mode = LOBOTOMIA;
		}
		break;

		// En este modo el motor se mantiene apagado
		case LOBOTOMIA:
		cbi(OUTRUT,M2_en);
		cbi(OUTRUT,M2_di);
		break;

	}

	// PCINT(S05) reseteo de posición, con antirrebote
	// Un tiempo de 1ms, la flag se activa en la interrupción
	if ( millis() - nowInter > 1 && flag) {
		// Si sigue en nivel alto el sensor, reseteamos la posición
		if (rbi(PINB,PB0))
			// Como el sensor esta descentrado, no reseteamos a 0
			pos = +6;
		// Bajamos la flag
		flag = 0;
	}

	// Boton de emergencia, activo nivel bajo
	if (!rbi(PINK,SW1)) {
		// Si estamos en rota
		if (mode == ROTA) {
			// Llamamos al callback de emergencia para cancelar el uso de monedero y tarjetero
            emergencia();
            // Pasamos al modo emergencia
			mode = EMERGENCIA;
			// Cambiamos de dirección para frenar a contra marchar
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			auxTime = millis();
		// En cuelga o en espera
		} else if (mode==CUELGA || mode==ESPERA) {
			// Pasamos al modo lobotomia
			mode = LOBOTOMIA;
			// Llamamos al callback de emergencia
            emergencia();
		// En frena
		} else if(mode == FRENA) {
			// Pasamos al modo lobotomia
			mode = EMERGENCIA;
			// Llamamos al callback de emergencia
			emergencia();
		// En gira
        } else if (mode == GIRA) {
			// Pasamos al modo catastrofre
			mode = CATASTROFE;
			// Cambiamos de dirección para frenar
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			auxTime = millis();
			// Llamamos al callback de emergencia
			emergencia();
		// En el modo carga
        } else if (mode == CARGA) {
			// Pasamos al modo lobotomia
			mode = LOBOTOMIA;
			// Apagamos L3
			cbi(OUTRUT,L3);
			// Llamamos al callback de emergencia
			emergencia();
        }
	}

	// Parpadeo de L4
	// En los modos siguientes, se mantiene encendido 0.5s y 10s apagado
	if (mode == CUELGA || mode == ROTA || mode == GIRA || mode == FRENA)
		parpadearL4(500,10000);
	// En los modos siguientes se mantiene encendido 0.2s y 1s apagado
	else if (mode == LOBOTOMIA ||mode == CATASTROFE||mode == EMERGENCIA )
		parpadearL4(200,1000);
	// Si no, apagamos el led
	else {
		cbi(OUTRUT,L4);
		L4on = true;
	}

#ifdef SERIAL_DEBUG
	// Debug por serial, muy cómodo con el plotter de Arduino
	static long int pocoPoco = 0;
	// Limitación de tiempo para no saturar el puerto
	if (millis() - pocoPoco > 5) {
		pocoPoco = millis();
		serialPrintLong(dif);
		serialWrite(' ');
		serialPrintLong(pos);
		serialWrite(' ');
		serialPrintUInt(bouncing_time);
		serialWrite(' ');
		serialPrintLong((dir) ? 50 : -50);
		serialWrite(' ');
		serialPrintInt(((int) mode)*100);
		serialWrite('\n');
	}
#endif
}
