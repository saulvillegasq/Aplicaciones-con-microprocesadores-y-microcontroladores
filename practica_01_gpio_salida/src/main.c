#include "pico/stdlib.h"

#define LED_PIN           PICO_DEFAULT_LED_PIN
#define LED_ACTIVE_LEVEL  1u

static void led_on(void)
{
    gpio_put(LED_PIN, LED_ACTIVE_LEVEL);
}

static void led_off(void)
{
    gpio_put(LED_PIN, !LED_ACTIVE_LEVEL);
}

int main(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true)
    {
        led_on();
        sleep_ms(500);

        led_off();
        sleep_ms(500);
    }
}
