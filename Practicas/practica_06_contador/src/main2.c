
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/time.h"
#include "hardware/sync.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

// ============================================================
// PRÁCTICA - MEDIDOR DE CICLO DE TRABAJO
// ============================================================
//
// Objetivo:
//   Medir el período, frecuencia y ciclo de trabajo
//   de una señal digital de aproximadamente 100 Hz.
//
// Hardware:
//   GP15 -> Entrada digital de 0 a 3.3 V.
//
// Funcionamiento:
//   1. Inicializar USB CDC.
//   2. Configurar GP15 como entrada digital.
//   3. Habilitar interrupciones por ambos flancos.
//   4. Capturar marcas temporales mediante TIMER.
//   5. Obtener período y tiempo en alto.
//   6. Enviar resultados cada segundo.
//
// La medición utiliza interrupciones GPIO y la
// base temporal del TIMER de hardware.
//
// ============================================================


// ------------------------------------------------------------
// 1. CONSTANTES
// ------------------------------------------------------------

#define SIGNAL_PIN          15u
#define REPORTE_MS          1000


// ------------------------------------------------------------
// 2. VARIABLES GLOBALES
// ------------------------------------------------------------

// Temporizador periódico utilizado para los reportes.
static struct repeating_timer timer;

// Marcas temporales de los flancos, en microsegundos.
static volatile uint64_t ultima_subida_us = 0u;
static volatile uint64_t ultima_bajada_us = 0u;

// Control de la secuencia de flancos.
static volatile bool subida_valida = false;
static volatile bool bajada_valida = false;

// Últimos intervalos completos medidos.
static volatile uint32_t periodo_us = 0u;
static volatile uint32_t tiempo_alto_us = 0u;

// Indica si existe una medición completa.
static volatile bool medicion_valida = false;

// Solicitud de reporte del TIMER.
static volatile bool reporte_pendiente = false;


// ------------------------------------------------------------
// 3. PROTOTIPOS
// ------------------------------------------------------------

static void configurar_medicion(void);
static void mostrar_resultados(void);

static void gpio_callback(uint gpio, uint32_t events);
static bool timer_callback(struct repeating_timer *rt);


// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 4.1 PUNTEROS A FUNCIÓN
    // --------------------------------------------------------

    // Seleccionar funciones de configuración
    // y procesamiento mediante punteros.

    void (*configurar)(void) = configurar_medicion;
    void (*procesar)(void) = mostrar_resultados;


    // --------------------------------------------------------
    // 4.2 INICIALIZACIÓN USB CDC
    // --------------------------------------------------------

    stdio_init_all();

    while (!stdio_usb_connected())
    {
        tight_loop_contents();
    }

    printf("\r\nMEDIDOR DE CICLO DE TRABAJO\r\n");
    printf("Entrada: GP15\r\n");
    printf("Reporte: cada 1000 ms\r\n");


    // --------------------------------------------------------
    // 4.3 CONFIGURACIÓN DE MEDICIÓN
    // --------------------------------------------------------

    // Ejecutar la función de configuración
    // a través de su puntero.
    configurar();


    // --------------------------------------------------------
    // 4.4 CONFIGURACIÓN DEL TIMER
    // --------------------------------------------------------

    // Programar una alarma periódica cada segundo.
    //
    // Este TIMER solamente solicita los reportes.
    // Las mediciones se actualizan en los flancos.

    bool ok = add_repeating_timer_ms(
        -REPORTE_MS,
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

    printf("Medicion iniciada.\r\n");


    // ========================================================
    // 5. CICLO PRINCIPAL
    // ========================================================

    while (true)
    {
        // Ejecutar el procesamiento seleccionado
        // mediante el puntero a función.
        procesar();

        tight_loop_contents();
    }
}


// ============================================================
// 6. CONFIGURACIÓN DEL GPIO
// ============================================================

static void configurar_medicion(void)
{
    // Inicializar GP15 como entrada digital.
    gpio_init(SIGNAL_PIN);
    gpio_set_dir(SIGNAL_PIN, GPIO_IN);

    // La señal procede de un generador externo.
    // No se requieren resistencias internas.
    gpio_disable_pulls(SIGNAL_PIN);

    // Habilitar interrupciones para ambos flancos:
    //
    // GPIO_IRQ_EDGE_RISE:
    //   Transición de nivel bajo a alto.
    //
    // GPIO_IRQ_EDGE_FALL:
    //   Transición de nivel alto a bajo.

    gpio_set_irq_enabled_with_callback(
        SIGNAL_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback
    );
}


// ============================================================
// 7. PROCESAMIENTO DE RESULTADOS
// ============================================================

static void mostrar_resultados(void)
{
    uint32_t periodo;
    uint32_t alto;

    bool reporte;
    bool valida;

    // --------------------------------------------------------
    // 7.1 COPIAR RESULTADOS DE FORMA CONSISTENTE
    // --------------------------------------------------------

    // Las interrupciones GPIO modifican las variables.
    // Por ello se deshabilitan brevemente las IRQ
    // mientras se copian los datos compartidos.

    uint32_t irq = save_and_disable_interrupts();

    reporte = reporte_pendiente;

    if (reporte)
    {
        reporte_pendiente = false;
    }

    valida = medicion_valida;

    periodo = periodo_us;
    alto = tiempo_alto_us;

    restore_interrupts(irq);


    // --------------------------------------------------------
    // 7.2 COMPROBAR DISPONIBILIDAD
    // --------------------------------------------------------

    if (!reporte)
    {
        return;
    }

    if (!valida || periodo == 0u)
    {
        printf("Esperando una medicion completa...\r\n");
        return;
    }


    // --------------------------------------------------------
    // 7.3 CALCULAR FRECUENCIA
    // --------------------------------------------------------

    // El TIMER entrega microsegundos.
    //
    // f = 1000000 / período_us
    //
    // El resultado se expresa en Hz.

    double frecuencia_hz =
        1000000.0 / (double)periodo;


    // --------------------------------------------------------
    // 7.4 CALCULAR CICLO DE TRABAJO
    // --------------------------------------------------------

    // D = (tiempo_alto / período) * 100
    //
    // D = 0 %   -> Señal siempre baja.
    // D = 50 %  -> Mitad del período en alto.
    // D = 100 % -> Señal siempre alta.
    //
    // Este algoritmo necesita flancos, por lo que
    // una señal constante no proporciona un período
    // medible mediante este procedimiento.

    double duty =
        ((double)alto / (double)periodo) * 100.0;


    // --------------------------------------------------------
    // 7.5 ENVIAR RESULTADOS POR USB
    // --------------------------------------------------------

    printf("\r\n");
    printf("========================================\r\n");
    printf(" RESULTADO DE MEDICION\r\n");
    printf("========================================\r\n");

    printf("Periodo: %" PRIu32 " us\r\n", periodo);

    printf("Tiempo alto: %" PRIu32 " us\r\n", alto);

    printf("Frecuencia: %.2f Hz\r\n", frecuencia_hz);

    printf("Ciclo de trabajo: %.2f %%\r\n", duty);

    printf("========================================\r\n");
}


// ============================================================
// 8. INTERRUPCIÓN GPIO
// ============================================================

static void gpio_callback(uint gpio, uint32_t events)
{
    // Comprobar el GPIO que produjo la interrupción.
    if (gpio != SIGNAL_PIN)
    {
        return;
    }

    // Leer una marca temporal del TIMER de hardware.
    //
    // Se registra el momento en que el firmware
    // atiende la interrupción.
    uint64_t ahora_us = time_us_64();


    // --------------------------------------------------------
    // 8.1 FLANCO ASCENDENTE
    // --------------------------------------------------------

    if (events & GPIO_IRQ_EDGE_RISE)
    {
        // El nuevo flanco ascendente permite
        // determinar el período completo.

        if (subida_valida && bajada_valida)
        {
            uint64_t periodo =
                ahora_us - ultima_subida_us;

            uint64_t alto =
                ultima_bajada_us - ultima_subida_us;

            // Verificar coherencia de los intervalos.
            if (periodo > 0u &&
                periodo <= UINT32_MAX &&
                alto <= periodo)
            {
                periodo_us = (uint32_t)periodo;
                tiempo_alto_us = (uint32_t)alto;

                medicion_valida = true;
            }
        }

        // Guardar el nuevo comienzo de período.
        ultima_subida_us = ahora_us;

        subida_valida = true;
        bajada_valida = false;
    }


    // --------------------------------------------------------
    // 8.2 FLANCO DESCENDENTE
    // --------------------------------------------------------

    if (events & GPIO_IRQ_EDGE_FALL)
    {
        // El tiempo en alto finaliza cuando
        // ocurre el flanco descendente.

        if (subida_valida && !bajada_valida)
        {
            ultima_bajada_us = ahora_us;

            bajada_valida = true;
        }
    }
}


// ============================================================
// 9. CALLBACK DEL TIMER PERIÓDICO
// ============================================================

static bool timer_callback(struct repeating_timer *rt)
{
    (void)rt;

    // Notificar al programa principal que ha
    // transcurrido otro segundo de observación.
    //
    // No realizar printf() dentro del callback.

    reporte_pendiente = true;

    // Mantener la alarma periódica habilitada.
    return true;
}
