#include "pico/stdlib.h"
#include <stdint.h>
#include "led.h"

#define PERIOD_A_MS 200u
#define PERIOD_B_MS 750u

int main(void)
{
    led_init();

    uint32_t last_a = to_ms_since_boot(get_absolute_time());
    uint32_t last_b = last_a;

    while (true)
    {
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if ((now - last_a) >= PERIOD_A_MS)
        {
            last_a = now;
            led_toggle();
        }

        if ((now - last_b) >= PERIOD_B_MS)
        {
            last_b = now;

            /*
             * TODO:
             * Agregar una segunda salida para observar dos procesos temporales
             * sin utilizar retardos secuenciales bloqueantes.
             */
        }
    }
}
