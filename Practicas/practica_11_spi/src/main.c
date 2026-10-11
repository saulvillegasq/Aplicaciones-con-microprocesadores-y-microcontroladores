#include "pico/stdlib.h"
#include "spi_bus.h"

int main(void)
{
    spi_bus_init();

    while (true)
    {
        /*
         * TODO:
         * Transferir datos y observar SCK/MOSI/MISO/CS.
         */
        tight_loop_contents();
    }
}
