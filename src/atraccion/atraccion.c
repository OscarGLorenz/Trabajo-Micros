#include "time.h"
#include "macros.h"
#include "pinout.h"
#include "serial.h"

#include "atraccion.h"

// Callback de atraccion finalizada
static void (*callback) ();
void atraccionSetCallbackFinalizado(void(*f)()) {
	callback = f;
}

// Variables auxiliares para el encoder
static volatile long int pos = 0;
static volatile bool dir = 0;
static volatile long int now = 0;
static volatile long int past = 0;
static volatile long int dif = 0;
static volatile long int lastIn = 0;
static float times[5];
static unsigned int bouncing_time=500;

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
	now = millis();
	dif = now - past;

	for (int j=0;j<4;j++){
		times[j]=times[j+1];
	}
	times[4] = dif;

	int aux = (((times[0]+times[1]) * 2.5) - (times[2]+times[3]+times[4]));

	if (now - lastIn > bouncing_time){
		if (aux < 0){

			if (pos > 40  || pos < -40 ){
				bouncing_time = 3 *(times[2]+times[3]+times[4]);
				if (bouncing_time < 500) bouncing_time = 500;
			}else bouncing_time = 500;

			dir = !dir;
			tbi(OUTRUT,M2_di); // Toggle M2_dir
			lastIn = now;
		}
	}

	past = now;
}

static volatile bool flag = 0;
static volatile long int nowInter = 0;
ISR(SO5_vect) {

	if (rbi(PIN_S05,P_S05)) {
		flag = 1;
		nowInter = millis();
	}
}

void parpadearL4(unsigned int on, unsigned int off) {
	static bool L4on = false;
	static unsigned int L4tiempo = 0 ;

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
		if (millis() - auxTime > 10000 ){
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
		}
		break;

		case ROTA:
		if (millis() - lastIn > 2000) {
			mode = GIRA;
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
		if (millis() - auxTime > 12000) {
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
			cbi(OUTRUT,L3); // Toggle M2_dir
		}
	}

	// L4
	if (mode == CUELGA ||mode == ROTA ||mode == GIRA ||mode == FRENA)
		parpadearL4(500,10000);
	else if (mode == LOBOTOMIA ||mode == CATASTROFE||mode == EMERGENCIA )
		parpadearL4(200,1000);
	else
		cbi(OUTRUT,L4);

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
