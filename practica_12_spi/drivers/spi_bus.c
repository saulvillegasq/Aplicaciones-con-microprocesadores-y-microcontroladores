#include "spi_bus.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_ID       spi0
#define SPI_SCK_PIN  18u
#define SPI_TX_PIN   19u
#define SPI_RX_PIN   16u
#define SPI_CS_PIN   17u

void spi_bus_init(void)
{
    spi_init(SPI_ID, 1000000u);

    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_CS_PIN);
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
    gpio_put(SPI_CS_PIN, 1);
}
