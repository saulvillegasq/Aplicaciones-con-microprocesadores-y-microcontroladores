#include "pico/stdlib.h"
#include "uart_link.h"

int main(void)
{
    uart_link_init();

    while (true)
    {
        uart_link_write("Hola desde RP2040\r\n");
        sleep_ms(1000);
    }
}
