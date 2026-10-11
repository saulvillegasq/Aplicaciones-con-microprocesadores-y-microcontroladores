
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/time.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// ============================================================
// PRÁCTICA 06 - CONTADOR DE PULSOS POR HARDWARE
// ============================================================
//
// Objetivo:
//   Contar los pulsos de una señal digital de 100 Hz
//   utilizando el contador del periférico PWM.
//
// Hardware:
//   GP15 -> Entrada de señal digital (0 a 3.3 V).
//
// Funcionamiento:
//   1. Inicializar USB CDC.
//   2. Configurar el PWM como contador de flancos.
//   3. Iniciar el contador de hardware.
//   4. Programar una alarma periódica de 1 segundo.
//   5. Leer y reiniciar el contador en cada ventana.
//   6. Enviar el resultado mediante printf().
//
// El conteo de pulsos lo realiza el hardware.
// No se incrementa un contador desde una ISR GPIO.
//
// ============================================================


// ------------------------------------------------------------
// 1. CONSTANTES
// ------------------------------------------------------------

#define SIGNAL_PIN       15u
#define VENTANA_MS       1000


// ------------------------------------------------------------
// 2. VARIABLES GLOBALES
// ------------------------------------------------------------

// Identificador del slice PWM correspondiente al GPIO.
static uint pwm_slice;

// Estructura administrada por el SDK para la alarma.
static struct repeating_timer timer;

// Resultado compartido entre el callback y main().
static volatile uint16_t pulsos = 0u;
static volatile bool dato_disponible = false;


// ------------------------------------------------------------
// 3. PROTOTIPOS
// ------------------------------------------------------------

static void configurar_contador(void);
static void mostrar_resultado(void);

static bool timer_callback(struct repeating_timer *rt);


// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 4.1 PUNTEROS A FUNCIÓN
    // --------------------------------------------------------

    // Las variables almacenan direcciones de funciones
    // cuya firma es: void funcion(void).

    void (*configurar)(void) = configurar_contador;
    void (*procesar)(void) = mostrar_resultado;


    // --------------------------------------------------------
    // 4.2 INICIALIZACIÓN USB
    // --------------------------------------------------------

    stdio_init_all();

    // Esperar a que se abra el monitor serial.
    while (!stdio_usb_connected())
    {
        tight_loop_contents();
    }

    printf("\r\nCONTADOR DE PULSOS\r\n");
    printf("Entrada: GP15\r\n");
    printf("Ventana: 1000 ms\r\n");


    // --------------------------------------------------------
    // 4.3 CONFIGURACIÓN DEL CONTADOR
    // --------------------------------------------------------

    // Ejecutar la función seleccionada mediante
    // el puntero a función.
    configurar();


    // --------------------------------------------------------
    // 4.4 ALARMA PERIÓDICA
    // --------------------------------------------------------

    // La alarma vence cada 1000 ms.
    //
    // El signo negativo indica que la planificación
    // se realiza respecto al inicio programado
    // de cada ejecución del callback.

    bool ok = add_repeating_timer_ms(
        -VENTANA_MS,
        timer_callback,
        NULL,
        &timer
    );

    if (!ok)
    {
        printf("ERROR: No se pudo crear el timer.\r\n");

        while (true)
        {
            tight_loop_contents();
        }
    }

    printf("Conteo iniciado.\r\n");


    // ========================================================
    // 5. CICLO PRINCIPAL
    // ========================================================

    while (true)
    {
        // El puntero ejecuta la función encargada
        // de presentar los resultados disponibles.
        procesar();

        tight_loop_contents();
    }
}


// ============================================================
// 6. CONFIGURAR PWM COMO CONTADOR
// ============================================================

static void configurar_contador(void)
{
    // GP15 corresponde al canal B de un slice PWM.
    //
    // El modo PWM_DIV_B_RISING permite contar
    // flancos ascendentes de la señal externa.

    pwm_slice = pwm_gpio_to_slice_num(SIGNAL_PIN);

    // Conectar GP15 a su función PWM.
    gpio_set_function(SIGNAL_PIN, GPIO_FUNC_PWM);

    // Obtener una configuración inicial del SDK.
    pwm_config config = pwm_get_default_config();

    // Contar flancos ascendentes del canal B.
    pwm_config_set_clkdiv_mode(
        &config,
        PWM_DIV_B_RISING
    );

    // Divisor igual a 1.
    pwm_config_set_clkdiv_int(&config, 1u);

    // Contador de 16 bits: valores de 0 a 65535.
    pwm_config_set_wrap(&config, 65535u);

    // Configurar sin habilitar todavía el contador.
    pwm_init(pwm_slice, &config, false);

    // Comenzar desde cero.
    pwm_set_counter(pwm_slice, 0u);

    // Iniciar el contador físico.
    pwm_set_enabled(pwm_slice, true);
}


// ============================================================
// 7. MOSTRAR RESULTADOS
// ============================================================

static void mostrar_resultado(void)
{
    uint16_t lectura;
    bool disponible;

    // Proteger la copia de las variables compartidas.
    uint32_t irq = save_and_disable_interrupts();

    disponible = dato_disponible;

    if (disponible)
    {
        lectura = pulsos;
        dato_disponible = false;
    }

    restore_interrupts(irq);

    // Solo enviar un resultado cuando haya terminado
    // una ventana completa de medición.
    if (disponible)
    {
        printf("\r\nPulsos contados: %u\r\n",
               (unsigned)lectura);

        // Como la ventana nominal es de un segundo,
        // la cantidad de pulsos aproxima la frecuencia.
        printf("Frecuencia aprox.: %u Hz\r\n",
               (unsigned)lectura);
    }
}


// ============================================================
// 8. CALLBACK DEL TIMER
// ============================================================

static bool timer_callback(struct repeating_timer *rt)
{
    (void)rt;

    // Detener momentáneamente el contador PWM.
    pwm_set_enabled(pwm_slice, false);

    // Obtener el conteo acumulado.
    pulsos = pwm_get_counter(pwm_slice);

    // Preparar la siguiente ventana.
    pwm_set_counter(pwm_slice, 0u);

    // Reanudar el contador de hardware.
    pwm_set_enabled(pwm_slice, true);

    // Avisar al programa principal.
    dato_disponible = true;

    // Mantener la alarma periódica activa.
    return true;
}
