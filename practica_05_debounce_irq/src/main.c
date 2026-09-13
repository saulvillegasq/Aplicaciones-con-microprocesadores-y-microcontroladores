#include "pico/stdlib.h"
#include <stdbool.h>
#include "led.h"

#define BUTTON_PIN          15u
#define DEBOUNCE_US         20000u

static volatile bool edge_event = false;
static volatile uint64_t edge_time_us = 0;

static void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL))
    {
        edge_time_us = time_us_64();
        edge_event = true;
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
        if (edge_event)
        {
            uint64_t now = time_us_64();

            if ((now - edge_time_us) >= DEBOUNCE_US)
            {
                edge_event = false;

                if (gpio_get(BUTTON_PIN) == 0)
                {
                    led_toggle();
                }
            }
        }
    }
}
