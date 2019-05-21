#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"

#include "atraccion.h"
#include <stdint.h>

// Callback de atraccion finalizada
static void (*callback) ();
void atraccionSetCallbackFinalizado(void(*f)()) {
	callback = f;
}

// Variables auxiliares para el encoder
static volatile int16_t pos = 0;
static volatile uint8_t dir = 0;
static volatile int32_t now = 0;
static volatile int32_t past = 0;
static volatile int32_t dif = 0;
static volatile int32_t lastIn = 0;
static volatile int32_t times[5];
static volatile int16_t bouncing_time=520;

enum mode_t {ESPERA,CUELGA,ROTA,GIRA,FRENA,EMERGENCIA,CATASTROFE,LOBOTOMIA,CARGA};
static enum mode_t mode = ESPERA;

static long int auxTime = 0;

void atraccionIniciar() {
	if (mode == ESPERA) {
		mode = CARGA;
		auxTime = millis();
		sbi(OUTRUT,L3);
	}
}

ISR(SO4_vect){

	if (dir) pos++;
	else pos--;

/*
        asm volatile(
        " push r16\n"
        " push r24\n"
        " push r25\n"
        " push r26\n"
        " push r27\n"
        " push r30\n"
        " push r31\n"

        " ld r24,Z+ \n"
        " ld r25, Z \n"

        " ld r16,X \n"
        " tst r16 \n"
        " breq zero \n"
        //sumar 1

        "adiw r24,1 \n"

        " jmp end\n"
        "zero: \n"

        "sbiw r24,1 \n"


        "end: \n"

        "sbiw z,1 \n"
        " st Z+,r24 \n"
        " st Z,r25 \n"

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
	now = millis();
	volatile uint32_t caca = now;
	dif = now - past;
/*        asm volatile(
        "push r26 \n"
        "push r27 \n"
        "push r30 \n"
        "push r31 \n"
        "push r16 \n"
        "push r17 \n"

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


    past = caca;


for (int j=0;j<4;j++)
		times[j]=times[j+1];
	times[4] = dif;


/*
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
        "movw  Y,X        \n"
        "adiw  Y,4        \n"
        "ldi r18,4        \n"
        "loop2:           \n"
        "   ldi r17,4     \n"
        "   loop1:        \n"
        "     ld r16, Y+  \n"
        "     st X+, r16  \n"
        "     dec r17     \n"
        "     tst r17     \n"
        "     brne loop1  \n"
        "   dec r18       \n"
        "   tst r18       \n"
        "   brne loop2    \n"
        "ldi r17,4        \n"
        "loop3:           \n"
        "   ld r16, Z+    \n"
        "   st X+, r16    \n"
        "   dec r17       \n"
        "   tst r17       \n"
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

      aux = (  ( (times[0]+times[1]) << 1) + ( (times[0]+times[1]) >> 1)  )  - (times[2]+times[3]+times[4])   ;

/*
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
);*/


	if (now - lastIn > bouncing_time){
		if (aux < 0){

/*

			 if (pos > 40  || pos < -40 ){
			 	bouncing_time = 3 *(times[2]+times[3]+times[4]);
				if (bouncing_time < 500) bouncing_time = 500;
			 } else bouncing_time = 500;

*/


			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			lastIn = now;
		}
	}


}

static volatile bool flag = 0;
static volatile long int nowInter = 0;
ISR(SO5_vect) {

	if (rbi(PIN_S05,P_S05)) {
		flag = 1;
		nowInter = millis();
	}
}

static bool L4on = true;
static unsigned int L4tiempo = 0 ;
void parpadearL4(unsigned int on, unsigned int off) {
	if (L4on){
		if (millis()-L4tiempo > on){
			L4on = false;
			cbi(OUTRUT,L4);
			L4tiempo = millis();
		}
	} else{
		if (millis()-L4tiempo > off){
			L4on = true;
			sbi(OUTRUT,L4);
			L4tiempo = millis();
		}
	}
}

void atraccionSetup() {
	cli();

	// CHANGE triggers INT3
	sbi(CTRL_INT, SO4_C0);
	cbi(CTRL_INT, SO4_C1);

	// Enable INT3
	sbi(INT_MASK, SO4);

	// Enable PCINT0
	sbi(S05_CTRL, SO5_ENABLE);
	sbi(SO5_MASK, SO5);

	sei();
}

void atraccionLoop() {
	// FSM
	switch(mode) {
		case ESPERA:
		break;

		case CARGA :
		if (millis() - auxTime > 1000 ){
			mode = CUELGA;
			sbi(OUTRUT,M2_en);
			cbi(OUTRUT,L3);
			auxTime= millis();
		}
		break;

		case CUELGA:
		if(pos < 10) {
			if (millis() - auxTime < 1000)
				cbi(OUTRUT,M2_di);
			else if (millis()%1200 > 1200/2)
				sbi(OUTRUT,M2_di);
			else
				cbi(OUTRUT,M2_di);
		} else {
			mode = ROTA;
			lastIn = now;
            auxTime= millis();
        }
		break;

		case ROTA:
		if (millis() - lastIn > 2000) {
			mode = GIRA;
		}
		// WATCHDOG
		if (millis() - auxTime > 100000 ) {
                mode = FRENA;
		}
		break;

		case GIRA:
		if (millis() - lastIn > 10000) {
			mode = FRENA;
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			auxTime = millis();
		}
		break;

		case FRENA:
		if (millis() - auxTime > 20000) {
			mode = ESPERA;
			cbi(OUTRUT,M2_en);
			cbi(OUTRUT,M2_di);
			callback();
		}
		break;

		case CATASTROFE:
		if (millis() - auxTime > 5000) {
			mode = EMERGENCIA;
			auxTime = millis();
		}
		break;

		case EMERGENCIA:
		if (millis() - auxTime > 5000) {
			mode = LOBOTOMIA;
		}
		break;

		case LOBOTOMIA:
		cbi(OUTRUT,M2_en);
		cbi(OUTRUT,M2_di);
		break;

	}

	// PCINT reseteo de posición, con antirrebote
	if ( millis() - nowInter > 1 && flag) {
		if (rbi(PINB,PB0))
			pos = +6;
		flag = 0;
	}

	// Boton de emergencia
	if (!rbi(PINK,SW1)) {
		if (mode == ROTA) {
			mode = EMERGENCIA;
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			auxTime = millis();
		} else if (mode==CUELGA || mode==ESPERA) {
			mode = LOBOTOMIA;
		} else if(mode == FRENA) {
			mode = EMERGENCIA;
		} else if (mode == GIRA) {
			mode = CATASTROFE;
			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			auxTime = millis();
		} else if (mode == CARGA) {
			mode = LOBOTOMIA;
			cbi(OUTRUT,L3); // Clear L3
		}
	}

	// L4
	if (mode == CUELGA ||mode == ROTA ||mode == GIRA ||mode == FRENA)
		parpadearL4(500,10000);
	else if (mode == LOBOTOMIA ||mode == CATASTROFE||mode == EMERGENCIA )
		parpadearL4(200,1000);
	else {
		cbi(OUTRUT,L4);
		L4on = true;
	}

	// Data dump
	static long int pocoPoco = 0;
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
}
