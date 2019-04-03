#include <avr/io.h>
#include <avr/interrupt.h>
#include "../charpy/time.h"
#include "../charpy/pinout.h"

using namespace charpy::time;


void setup() {
    OUTRUT = 0x01;

}

unsigned long i = 0;
void loop() {
    if (millis() - i > 500) {
        OUTRUT ^= 0x01;
        i = millis();
    }
}



int main() {
    init();
    setup();

    for(;;)
        loop();

    return 0;
}