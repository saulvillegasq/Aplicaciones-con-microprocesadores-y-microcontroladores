#include "i2c_bus.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_ID       i2c0
#define I2C_SDA_PIN  4u
#define I2C_SCL_PIN  5u
#define I2C_BAUD     100000u

void i2c_bus_init(void)
{
    i2c_init(I2C_ID, I2C_BAUD);

    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}
