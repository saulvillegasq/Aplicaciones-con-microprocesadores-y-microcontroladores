#include "pico/stdlib.h"
#include <stdio.h>

#define INPUT_PIN 15u

static volatile uint64_t last_time_us = 0;
static volatile uint64_t period_us = 0;

static void capture_callback(uint gpio, uint32_t events)
{
    if (gpio == INPUT_PIN && (events & GPIO_IRQ_EDGE_RISE))
    {
        uint64_t now = time_us_64();

        if (last_time_us != 0)
        {
            period_us = now - last_time_us;
        }

        last_time_us = now;
    }
}

int main(void)
{
    stdio_init_all();

    gpio_init(INPUT_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    gpio_pull_down(INPUT_PIN);

    gpio_set_irq_enabled_with_callback(
        INPUT_PIN,
        GPIO_IRQ_EDGE_RISE,
        true,
        &capture_callback
    );

    while (true)
    {
        /*
         * TODO:
         * Convertir period_us a frecuencia y observar el resultado.
         */
        tight_loop_contents();
    }
}
