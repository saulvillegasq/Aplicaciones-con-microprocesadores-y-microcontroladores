#include "pico/stdlib.h"

int main(void)
{
    /*
     * Esta práctica funciona como base para integrar el módulo inalámbrico
     * disponible en el laboratorio.
     *
     * El código concreto dependerá de si se utiliza:
     * - módulo Bluetooth externo,
     * - Pico W,
     * - módulo ZigBee,
     * - o una interfaz UART/SPI hacia un radio.
     */

    while (true)
    {
        tight_loop_contents();
    }
}
