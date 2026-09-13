#include "led.h"
#include "pico/stdlib.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

void led_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
}

void led_on(void)
{
    gpio_put(LED_PIN, 1);
}

void led_off(void)
{
    gpio_put(LED_PIN, 0);
}

void led_toggle(void)
{
    gpio_put(LED_PIN, !gpio_get_out_level(LED_PIN));
}
