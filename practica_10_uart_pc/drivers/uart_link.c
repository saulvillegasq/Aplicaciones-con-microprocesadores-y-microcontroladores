#include "uart_link.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID       uart0
#define UART_TX_PIN   0u
#define UART_RX_PIN   1u
#define UART_BAUD     115200u

void uart_link_init(void)
{
    uart_init(UART_ID, UART_BAUD);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void uart_link_write(const char *text)
{
    uart_puts(UART_ID, text);
}
