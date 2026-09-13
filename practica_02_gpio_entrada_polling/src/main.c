#include "pico/stdlib.h"
#include <stdbool.h>

#define LED_PIN             PICO_DEFAULT_LED_PIN
#define BUTTON_PIN          15u
#define BUTTON_ACTIVE_LEVEL 0u

int main(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true)
    {
        bool pressed = gpio_get(BUTTON_PIN) == BUTTON_ACTIVE_LEVEL;

        gpio_put(LED_PIN, pressed);

        /*
         * Actividad:
         * 1. Observar el comportamiento sin debounce.
         * 2. Implementar un contador de pulsaciones.
         * 3. Comprobar el rebote.
         * 4. Probar una espera corta y discutir por qué bloquea.
         */
    }
}
