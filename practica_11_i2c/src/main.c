#include "pico/stdlib.h"
#include "i2c_bus.h"

int main(void)
{
    i2c_bus_init();

    while (true)
    {
        /*
         * TODO:
         * Conectar un periférico y realizar una lectura/escritura.
         */
        tight_loop_contents();
    }
}
