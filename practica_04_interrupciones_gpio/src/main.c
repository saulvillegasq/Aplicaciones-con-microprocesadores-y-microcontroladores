#include "pico/stdlib.h"
#include <stdbool.h>
#include "led.h"

#define BUTTON_PIN 15u

static volatile bool button_event = false;

static void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL))
    {
        button_event = true;
    }
}

int main(void)
{
    led_init();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    gpio_set_irq_enabled_with_callback(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback
    );

    while (true)
    {
        if (button_event)
        {
            button_event = false;
            led_toggle();
        }
    }
}
