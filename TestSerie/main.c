#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

#include "../charpy/macros.h"
#include "../charpy/serial.h"
#include "../charpy/pinout.h"
#include "../charpy/time.h"


void setup() {
    initTime();
    serialBegin(9600);
}

void loop() {
    char buf[80];
    dtostrf(sin(micros()/1000.0),8,4,buf);
    serialPrintTab(buf);
    dtostrf(sin(millis()/1000.0 + 2*M_PI/3.0),8,4,buf);
    serialPrintTab(buf);
    dtostrf(sin(((long)millis())/1000.0 - 2*M_PI/3.0),8,4,buf);
    serialPrintLn(buf);
}

int main() {
    setup();

    for(;;)
        loop();

    return 0;
}
