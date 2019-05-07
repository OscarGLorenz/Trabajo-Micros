#include "monedero/monedero.h"

int main() {
    monederoSetup();
    for(;;) monederoLoop();
    return 0;
}