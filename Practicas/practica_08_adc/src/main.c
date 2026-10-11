#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#define ADC_GPIO 26u
#define ADC_CH   0u
#define VREF     3.3f

int main(void)
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_CH);

    while (true)
    {
        uint16_t raw = adc_read();
        float voltage = ((float)raw * VREF) / 4095.0f;

        printf("ADC=%u, V=%.3f\n", raw, voltage);

        sleep_ms(500);
    }
}
