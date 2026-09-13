#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_PIN 16u

int main(void)
{
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(PWM_PIN);

    pwm_set_wrap(slice, 999u);
    pwm_set_clkdiv(slice, 125.0f);

    pwm_set_gpio_level(PWM_PIN, 500u);
    pwm_set_enabled(slice, true);

    while (true)
    {
        /*
         * TODO:
         * Modificar duty cycle y medir la señal.
         */
        tight_loop_contents();
    }
}
