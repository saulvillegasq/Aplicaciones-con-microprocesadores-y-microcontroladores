#include "button.h"
#include "pico/stdlib.h"

#define BUTTON_PIN          15u
#define BUTTON_ACTIVE_LEVEL 0u

void button_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

bool button_is_pressed(void)
{
    return gpio_get(BUTTON_PIN) == BUTTON_ACTIVE_LEVEL;
}
