#include "monedero/monedero.h"

int main() {
    setupMonedero();
    for(;;) loopMonedero();
    return 0;
}