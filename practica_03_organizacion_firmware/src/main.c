#include "pico/stdlib.h"
#include "led.h"
#include "button.h"

int main(void)
{
    led_init();
    button_init();

    while (true)
    {
        if (button_is_pressed())
        {
            led_on();
        }
        else
        {
            led_off();
        }
    }
}
