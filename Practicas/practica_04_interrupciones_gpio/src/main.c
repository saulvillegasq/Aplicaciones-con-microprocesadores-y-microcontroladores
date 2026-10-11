
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>

// ============================================================
// PRÁCTICA 04 - POLLING E INTERRUPCIONES
// ============================================================
//
// Objetivo:
//   Controlar un LED mediante dos botones, comparando
//   dos mecanismos de detección de entradas digitales.
//
// Funcionamiento:
//   GP14 -> Enciende el LED mediante polling.
//   GP15 -> Apaga el LED mediante interrupción.
//
// Conexiones:
//   Botón de encendido: GP14 y GND.
//   Botón de apagado:   GP15 y GND.
//   LED: GP16, resistencia limitadora y GND.
//
// Ambos botones utilizan resistencias pull-up internas.
//
// ============================================================


// ------------------------------------------------------------
// 1. DEFINICIÓN DE CONSTANTES
// ------------------------------------------------------------

// Pines GPIO.
#define LED_PIN          16u
#define BUTTON_ON_PIN    14u
#define BUTTON_OFF_PIN   15u

// Niveles lógicos.
#define LED_ON           1u
#define LED_OFF          0u
#define BUTTON_PRESSED   0u

// Tiempo de antirrebote en milisegundos.
#define DEBOUNCE_MS      30u


// ------------------------------------------------------------
// 2. VARIABLE COMPARTIDA CON LA INTERRUPCIÓN
// ------------------------------------------------------------

// La interrupción modifica esta bandera cuando
// detecta una pulsación del botón de apagado.
//
// volatile indica que su valor puede cambiar
// de forma asíncrona respecto al programa principal.

static volatile bool button_off_event = false;


// ------------------------------------------------------------
// 3. PROTOTIPO DE LA FUNCIÓN DE INTERRUPCIÓN
// ------------------------------------------------------------

static void routine_int(uint gpio, uint32_t events);


// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 4.1 INICIALIZACIÓN DEL LED
    // --------------------------------------------------------

    // Inicializar GP16 para utilizarlo como GPIO.
    gpio_init(LED_PIN);

    // Preparar el nivel bajo antes de habilitar la salida.
    gpio_put(LED_PIN, LED_OFF);

    // Configurar GP16 como salida digital.
    gpio_set_dir(LED_PIN, GPIO_OUT);


    // --------------------------------------------------------
    // 4.2 CONFIGURACIÓN DEL BOTÓN DE ENCENDIDO
    // --------------------------------------------------------

    // GP14 será consultado directamente desde main().
    gpio_init(BUTTON_ON_PIN);
    gpio_set_dir(BUTTON_ON_PIN, GPIO_IN);

    // Activar la resistencia pull-up interna.
    gpio_pull_up(BUTTON_ON_PIN);


    // --------------------------------------------------------
    // 4.3 CONFIGURACIÓN DEL BOTÓN DE APAGADO
    // --------------------------------------------------------

    // GP15 generará una interrupción al presionarse.
    gpio_init(BUTTON_OFF_PIN);
    gpio_set_dir(BUTTON_OFF_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_OFF_PIN);


    // --------------------------------------------------------
    // 4.4 CONFIGURACIÓN DE LA INTERRUPCIÓN
    // --------------------------------------------------------

    // GPIO_IRQ_EDGE_FALL detecta el cambio de 1 a 0.
    //
    // Cuando se presiona el botón conectado a GND,
    // el nivel del GPIO pasa de alto a bajo.
    //
    // Al ocurrir este evento se ejecuta gpio_callback().

    gpio_set_irq_enabled_with_callback(
        BUTTON_OFF_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &routine_int
    );


    // ========================================================
    // 5. CICLO PRINCIPAL
    // ========================================================

    while (true)
    {
        // ----------------------------------------------------
        // 5.1 APAGADO SOLICITADO POR INTERRUPCIÓN
        // ----------------------------------------------------

        // La interrupción establece una bandera.
        // El programa principal atiende esa solicitud.

        if (button_off_event)
        {
            button_off_event = false;

            // Apagar el LED.
            gpio_put(LED_PIN, LED_OFF);
        }


        // ----------------------------------------------------
        // 5.2 ENCENDIDO MEDIANTE POLLING
        // ----------------------------------------------------

        // Se consulta continuamente el estado de GP14.
        //
        // El botón es activo en nivel bajo:
        //   1 -> Liberado.
        //   0 -> Presionado.

        if (gpio_get(BUTTON_ON_PIN) == BUTTON_PRESSED)
        {
            // Pequeña espera para reducir el efecto
            // del rebote mecánico del botón.
            sleep_ms(DEBOUNCE_MS);

            // Verificar que continúa presionado.
            if (gpio_get(BUTTON_ON_PIN) == BUTTON_PRESSED)
            {
                // Encender el LED.
                gpio_put(LED_PIN, LED_ON);
            }
        }

        // El ciclo se repite mientras la Pico
        // permanezca energizada.
    }
}


// ============================================================
// 6. FUNCIÓN DE ATENCIÓN DE LA INTERRUPCIÓN
// ============================================================

static void routine_int(uint gpio, uint32_t events)
{
    // Comprobar que el evento procede del botón GP15
    // y corresponde a un flanco de bajada.

    if ((gpio == BUTTON_OFF_PIN) &&
        (events & GPIO_IRQ_EDGE_FALL))
    {
        // Registrar el evento de apagado.
        //
        // La ISR no ejecuta retardos ni modificaciones
        // adicionales del hardware.
        //
        // La operación sobre el LED se realizará
        // posteriormente en el ciclo principal.

        button_off_event = true;
    }
}
