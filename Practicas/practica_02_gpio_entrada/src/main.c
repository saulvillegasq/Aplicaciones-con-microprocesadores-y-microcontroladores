
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>

// ============================================================
// PRÁCTICA 02 - PIEDRA, PAPEL O TIJERA
// ============================================================
//
// Objetivo:
//   Implementar un juego utilizando un botón y tres LED.
//
// Funcionamiento:
//   1. El programa espera a que se presione el botón.
//   2. El instante de pulsación determina la jugada
//      del jugador mediante un contador.
//   3. La Pico genera una segunda jugada.
//   4. Se comparan ambas selecciones.
//   5. Un LED indica el resultado.
//   6. Se espera otra pulsación para iniciar nuevamente.
//
// Este programa utiliza polling y retardos bloqueantes.
//
// ============================================================


// ------------------------------------------------------------
// 1. DEFINICIÓN DE PINES
// ------------------------------------------------------------

#define BUTTON_PIN       15u

#define LED_GANAR        16u
#define LED_PERDER       17u
#define LED_EMPATE       18u


// ------------------------------------------------------------
// 2. CONSTANTES DEL JUEGO
// ------------------------------------------------------------

#define PIEDRA           0u
#define PAPEL            1u
#define TIJERA           2u

#define BUTTON_PRESSED   0u

#define LED_ON           1u
#define LED_OFF          0u

#define DEBOUNCE_MS      30u


// ============================================================
// 3. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // Variables para almacenar las jugadas.
    uint8_t jugador = PIEDRA;
    uint8_t computadora = PIEDRA;

    // El contador cambia mientras se espera
    // una nueva pulsación.
    uint32_t contador = 0u;


    // --------------------------------------------------------
    // 3.1 CONFIGURACIÓN DEL BOTÓN
    // --------------------------------------------------------

    // Inicializar GPIO 15.
    gpio_init(BUTTON_PIN);

    // Configurar como entrada digital.
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // Activar resistencia pull-up interna.
    //
    // Botón liberado  -> GPIO = 1
    // Botón presionado -> GPIO = 0
    //
    // El pulsador debe conectarse entre GP15 y GND.
    gpio_pull_up(BUTTON_PIN);


    // --------------------------------------------------------
    // 3.2 CONFIGURACIÓN DE LOS LED
    // --------------------------------------------------------

    gpio_init(LED_GANAR);
    gpio_init(LED_PERDER);
    gpio_init(LED_EMPATE);

    // Preparar los tres LED apagados.
    gpio_put(LED_GANAR, LED_OFF);
    gpio_put(LED_PERDER, LED_OFF);
    gpio_put(LED_EMPATE, LED_OFF);

    // Configurar como salidas digitales.
    gpio_set_dir(LED_GANAR, GPIO_OUT);
    gpio_set_dir(LED_PERDER, GPIO_OUT);
    gpio_set_dir(LED_EMPATE, GPIO_OUT);


    // ========================================================
    // 4. CICLO PRINCIPAL
    // ========================================================

    while (true)
    {
        // ----------------------------------------------------
        // 4.1 ESPERAR EL INICIO DE UNA PARTIDA
        // ----------------------------------------------------

        // El contador se incrementa continuamente
        // mientras el botón permanece liberado.
        //
        // Su valor dependerá del instante en que
        // el jugador presione el botón.

        if (gpio_get(BUTTON_PIN) != BUTTON_PRESSED)
        {
            contador++;
        }
        else
        {
            // ------------------------------------------------
            // 4.2 COMPROBAR LA PULSACIÓN
            // ------------------------------------------------

            // Esperar para reducir el efecto del rebote.
            sleep_ms(DEBOUNCE_MS);

            // Confirmar que el botón continúa presionado.
            if (gpio_get(BUTTON_PIN) == BUTTON_PRESSED)
            {
                // --------------------------------------------
                // 4.3 SELECCIONAR LAS JUGADAS
                // --------------------------------------------

                // El residuo de dividir el contador entre 3
                // produce uno de los valores: 0, 1 o 2.
                jugador = (uint8_t)(contador % 3u);

                // La Pico utiliza el tiempo transcurrido
                // en microsegundos para obtener otro valor.
                //
                // Esta técnica es didáctica: no garantiza
                // aleatoriedad ni independencia estadística
                // entre las jugadas.
                computadora =
                    (uint8_t)(time_us_64() % 3u);


                // --------------------------------------------
                // 4.4 BORRAR EL RESULTADO ANTERIOR
                // --------------------------------------------

                gpio_put(LED_GANAR, LED_OFF);
                gpio_put(LED_PERDER, LED_OFF);
                gpio_put(LED_EMPATE, LED_OFF);


                // --------------------------------------------
                // 4.5 DETERMINAR EL RESULTADO
                // --------------------------------------------

                // EMPATE:
                // Ambas selecciones son iguales.

                if (jugador == computadora)
                {
                    gpio_put(LED_EMPATE, LED_ON);
                }

                // VICTORIA:
                // Piedra vence a tijera.
                // Papel vence a piedra.
                // Tijera vence a papel.

                else if (
                    (jugador == PIEDRA &&
                     computadora == TIJERA) ||

                    (jugador == PAPEL &&
                     computadora == PIEDRA) ||

                    (jugador == TIJERA &&
                     computadora == PAPEL))
                {
                    gpio_put(LED_GANAR, LED_ON);
                }

                // DERROTA:
                // Si no hay empate ni victoria,
                // el jugador ha perdido.

                else
                {
                    gpio_put(LED_PERDER, LED_ON);
                }


                // --------------------------------------------
                // 4.6 ESPERAR A LIBERAR EL BOTÓN
                // --------------------------------------------

                // Una misma pulsación no debe generar
                // varias partidas consecutivas.

                while (gpio_get(BUTTON_PIN) == BUTTON_PRESSED)
                {
                    sleep_ms(1);
                }

                // Reducir el efecto del rebote al soltar.
                sleep_ms(DEBOUNCE_MS);

                // Preparar el contador de la siguiente ronda.
                contador = 0u;

                // El LED del resultado permanece encendido
                // hasta que comience otra partida.
            }
        }
    }
}
