
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// ============================================================
// PRÁCTICA 07 - GENERACIÓN Y CONTROL DE PWM
// ============================================================
//
// Objetivo:
//   Generar una señal PWM mediante el hardware de la
//   Raspberry Pi Pico y modificar su ciclo de trabajo
//   utilizando dos botones.
//
// Funcionamiento:
//
//   GP14 -> Incrementar ciclo de trabajo.
//   GP15 -> Decrementar ciclo de trabajo.
//   GP16 -> Salida de señal PWM.
//
// Características:
//
//   Frecuencia PWM: 1000 Hz.
//   Ciclo de trabajo inicial: 50 %.
//   Incremento/decremento: 5 puntos porcentuales.
//   Ciclo mínimo: 0 %.
//   Ciclo máximo: 100 %.
//
// Cada modificación se muestra mediante printf()
// a través de USB CDC.
//
// Los botones utilizan resistencias pull-up internas.
// El antirrebote se realiza mediante polling y TIMER.
//
// ============================================================


// ------------------------------------------------------------
// 1. DEFINICIÓN DE PINES
// ------------------------------------------------------------

#define PWM_PIN             16u

#define BUTTON_UP_PIN       14u
#define BUTTON_DOWN_PIN     15u


// ------------------------------------------------------------
// 2. CONFIGURACIÓN DE PWM
// ------------------------------------------------------------

// Frecuencia deseada de la señal.
#define PWM_FREQUENCY_HZ    1000u

// Valor máximo del contador PWM.
//
// El contador recorre los valores de 0 a 9999.
// Esto proporciona 10000 cuentas por período.
#define PWM_WRAP            9999u

// Configuración del ciclo de trabajo en porcentaje.
#define DUTY_INITIAL        50u
#define DUTY_STEP           5u
#define DUTY_MIN            0u
#define DUTY_MAX            100u


// ------------------------------------------------------------
// 3. CONFIGURACIÓN DE BOTONES
// ------------------------------------------------------------

// Los botones están conectados entre GPIO y GND.
#define BUTTON_PRESSED      0u

// Tiempo de estabilidad para aceptar un cambio.
#define DEBOUNCE_US         30000u


// ------------------------------------------------------------
// 4. VARIABLES GLOBALES
// ------------------------------------------------------------

// Slice PWM asociado al GPIO seleccionado.
static uint pwm_slice;

// Ciclo de trabajo actual.
static uint8_t duty_cycle = DUTY_INITIAL;


// ------------------------------------------------------------
// 5. PROTOTIPOS DE FUNCIONES
// ------------------------------------------------------------

static void pwm_init_hardware(void);
static void pwm_set_duty(uint8_t duty);
static void pwm_print_status(void);


// ============================================================
// 6. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 6.1 VARIABLES PARA LOS BOTONES
    // --------------------------------------------------------

    // Estados anteriores y actuales de los botones.
    bool up_last_read = true;
    bool down_last_read = true;

    bool up_stable = true;
    bool down_stable = true;

    bool up_current;
    bool down_current;

    // Instantes del último cambio eléctrico observado.
    uint64_t up_change_us = 0u;
    uint64_t down_change_us = 0u;

    // Instante actual del temporizador de hardware.
    uint64_t ahora_us;


    // --------------------------------------------------------
    // 6.2 INICIALIZACIÓN USB CDC
    // --------------------------------------------------------

    // Inicializar printf() mediante USB.
    stdio_init_all();

    // Esperar hasta que el monitor serial se conecte.
    while (!stdio_usb_connected())
    {
        tight_loop_contents();
    }


    // --------------------------------------------------------
    // 6.3 CONFIGURACIÓN DE BOTONES
    // --------------------------------------------------------

    // Inicializar GPIO de incremento.
    gpio_init(BUTTON_UP_PIN);
    gpio_set_dir(BUTTON_UP_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_UP_PIN);

    // Inicializar GPIO de decremento.
    gpio_init(BUTTON_DOWN_PIN);
    gpio_set_dir(BUTTON_DOWN_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_DOWN_PIN);

    // Guardar los estados iniciales de ambos botones.
    up_last_read = gpio_get(BUTTON_UP_PIN);
    down_last_read = gpio_get(BUTTON_DOWN_PIN);

    up_stable = up_last_read;
    down_stable = down_last_read;


    // --------------------------------------------------------
    // 6.4 CONFIGURACIÓN DEL PWM
    // --------------------------------------------------------

    // Ejecutar la configuración del periférico PWM.
    pwm_init_hardware();

    // Establecer el ciclo de trabajo inicial.
    pwm_set_duty(duty_cycle);


    // --------------------------------------------------------
    // 6.5 MENSAJE INICIAL
    // --------------------------------------------------------

    printf("\r\n");
    printf("========================================\r\n");
    printf(" GENERADOR PWM - RASPBERRY PI PICO\r\n");
    printf("========================================\r\n");

    printf("Frecuencia: %u Hz\r\n", PWM_FREQUENCY_HZ);
    printf("Incremento: %u %%\r\n", DUTY_STEP);

    printf("GP14: Aumentar ciclo de trabajo\r\n");
    printf("GP15: Disminuir ciclo de trabajo\r\n");
    printf("GP16: Salida PWM\r\n");

    pwm_print_status();


    // ========================================================
    // 7. CICLO PRINCIPAL
    // ========================================================

    while (true)
    {
        // ----------------------------------------------------
        // 7.1 LEER EL TEMPORIZADOR DE HARDWARE
        // ----------------------------------------------------

        // time_us_64() obtiene la base de tiempo
        // del TIMER físico del microcontrolador.
        //
        // No se cuentan instrucciones ni iteraciones
        // del programa para medir los 30 ms.

        ahora_us = time_us_64();


        // ----------------------------------------------------
        // 7.2 LEER LOS DOS BOTONES
        // ----------------------------------------------------

        up_current = gpio_get(BUTTON_UP_PIN);
        down_current = gpio_get(BUTTON_DOWN_PIN);


        // ----------------------------------------------------
        // 7.3 DETECTAR CAMBIOS ELÉCTRICOS
        // ----------------------------------------------------

        // Si cambia la lectura, guardar el instante
        // en que se detectó la transición.

        if (up_current != up_last_read)
        {
            up_change_us = ahora_us;
            up_last_read = up_current;
        }

        if (down_current != down_last_read)
        {
            down_change_us = ahora_us;
            down_last_read = down_current;
        }


        // ----------------------------------------------------
        // 7.4 ANTIRREBOTE DEL BOTÓN DE INCREMENTO
        // ----------------------------------------------------

        // Aceptar el nuevo estado solamente después
        // de permanecer estable durante 30 ms.

        if ((ahora_us - up_change_us) >= DEBOUNCE_US)
        {
            if (up_current != up_stable)
            {
                up_stable = up_current;

                // Se genera una acción únicamente cuando
                // el botón cambia a estado presionado.
                //
                // Mantenerlo presionado no provoca
                // incrementos repetidos.

                if (up_stable == BUTTON_PRESSED)
                {
                    // Evitar superar el límite del 100 %.
                    if (duty_cycle < DUTY_MAX)
                    {
                        duty_cycle += DUTY_STEP;

                        if (duty_cycle > DUTY_MAX)
                        {
                            duty_cycle = DUTY_MAX;
                        }

                        // Actualizar el PWM de hardware.
                        pwm_set_duty(duty_cycle);

                        // Mostrar el nuevo valor.
                        pwm_print_status();
                    }
                }
            }
        }


        // ----------------------------------------------------
        // 7.5 ANTIRREBOTE DEL BOTÓN DE DECREMENTO
        // ----------------------------------------------------

        if ((ahora_us - down_change_us) >= DEBOUNCE_US)
        {
            if (down_current != down_stable)
            {
                down_stable = down_current;

                if (down_stable == BUTTON_PRESSED)
                {
                    // Evitar valores por debajo de 0 %.
                    if (duty_cycle >= DUTY_STEP)
                    {
                        duty_cycle -= DUTY_STEP;
                    }
                    else
                    {
                        duty_cycle = DUTY_MIN;
                    }

                    // Aplicar el nuevo ciclo de trabajo.
                    pwm_set_duty(duty_cycle);

                    // Informar por consola.
                    pwm_print_status();
                }
            }
        }

        // El hardware PWM continúa generando la señal
        // de manera independiente del ciclo principal.
    }
}


// ============================================================
// 8. CONFIGURACIÓN DEL PERIFÉRICO PWM
// ============================================================

static void pwm_init_hardware(void)
{
    // --------------------------------------------------------
    // 8.1 ASIGNAR EL GPIO AL PERIFÉRICO PWM
    // --------------------------------------------------------

    // GPIO_FUNC_PWM conecta el pin con el PWM físico.
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    // Identificar el slice PWM correspondiente.
    pwm_slice = pwm_gpio_to_slice_num(PWM_PIN);


    // --------------------------------------------------------
    // 8.2 OBTENER CONFIGURACIÓN INICIAL
    // --------------------------------------------------------

    pwm_config config = pwm_get_default_config();


    // --------------------------------------------------------
    // 8.3 CONFIGURAR EL PERÍODO
    // --------------------------------------------------------

    // El contador PWM utiliza valores de 0 a PWM_WRAP.
    //
    // La cantidad de cuentas por período es:
    //
    //     PWM_WRAP + 1
    //
    // La frecuencia también depende del divisor
    // aplicado al reloj del sistema.

    pwm_config_set_wrap(&config, PWM_WRAP);


    // --------------------------------------------------------
    // 8.4 CONFIGURAR EL DIVISOR DE RELOJ
    // --------------------------------------------------------

    // Obtener la frecuencia real del reloj del sistema.
    //
    // Esto evita asumir que Pico y Pico 2 utilizan
    // necesariamente la misma frecuencia de CPU.

    uint32_t frecuencia_sistema = clock_get_hz(clk_sys);

    // Calcular el divisor requerido.
    //
    // PWM_f = clk_sys / [divisor * (TOP + 1)]
    //
    // El divisor del PWM se configura con precisión
    // fraccionaria limitada por el hardware.

    float divisor =
        (float)frecuencia_sistema /
        ((float)PWM_FREQUENCY_HZ * (PWM_WRAP + 1u));

    pwm_config_set_clkdiv(&config, divisor);


    // --------------------------------------------------------
    // 8.5 INICIALIZAR PWM
    // --------------------------------------------------------

    // false indica que todavía no se inicia el PWM.
    pwm_init(pwm_slice, &config, false);

    // Iniciar contador de PWM desde cero.
    pwm_set_counter(pwm_slice, 0u);

    // Habilitar la generación mediante hardware.
    pwm_set_enabled(pwm_slice, true);
}


// ============================================================
// 9. MODIFICAR CICLO DE TRABAJO
// ============================================================

static void pwm_set_duty(uint8_t duty)
{
    // El ciclo de trabajo se configura mediante
    // el registro de comparación del PWM.
    //
    // El nivel de comparación depende de:
    //
    //   - Número de cuentas por período.
    //   - Porcentaje de ciclo de trabajo solicitado.
    //
    // El PWM mantiene fija su frecuencia mientras
    // se actualiza la duración del estado alto.

    uint16_t nivel =
        (uint16_t)(((uint32_t)duty * (PWM_WRAP + 1u))
                   / 100u);

    // Actualizar el canal asociado al GPIO.
    pwm_set_gpio_level(PWM_PIN, nivel);
}


// ============================================================
// 10. MOSTRAR INFORMACIÓN POR CONSOLA
// ============================================================

static void pwm_print_status(void)
{
    // Informar únicamente cuando existe un cambio
    // válido en el ciclo de trabajo.

    printf("Ciclo de trabajo actual: %u %%\r\n",
           (unsigned)duty_cycle);
}
