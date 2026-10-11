
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/time.h"
#include "hardware/timer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

// ============================================================
// PRÁCTICA 05 - TEMPORIZADORES DE HARDWARE
// ============================================================
//
// Objetivo:
//   Aprender a programar alarmas de hardware mediante
//   el Raspberry Pi Pico SDK y medir el tiempo de respuesta
//   de un usuario al presionar un botón.
//
// Secuencia de funcionamiento:
//
//   1. Inicializar el microcontrolador y USB CDC.
//   2. Esperar la conexión con el puerto COM.
//   3. Programar una alarma de 2 segundos.
//   4. Enviar un mensaje de sistema listo.
//   5. Programar una alarma de 4 segundos.
//   6. Comenzar la medición temporal.
//   7. Esperar la pulsación de un botón.
//   8. Registrar el instante mediante interrupción GPIO.
//   9. Obtener el tiempo transcurrido.
//  10. Enviar el resultado por USB CDC.
//
// Hardware:
//   GP15 -> Pulsador conectado a GND.
//
// Consideraciones:
//   - Se utiliza la resistencia pull-up interna.
//   - El botón es activo en nivel bajo.
//   - Las esperas utilizan alarmas del TIMER.
//   - La medición utiliza el contador de hardware.
//   - No se utilizan retardos sleep_ms().
//   - El programa realiza una medición por arranque.
//
// ============================================================


// ------------------------------------------------------------
// 1. DEFINICIÓN DE CONSTANTES
// ------------------------------------------------------------

// Pin utilizado como entrada digital.
#define BUTTON_PIN               15u

// Nivel lógico correspondiente al botón presionado.
#define BUTTON_PRESSED           0u

// Tiempos de las alarmas, expresados en milisegundos.
#define TIEMPO_INICIAL_MS        2000u
#define TIEMPO_PREPARACION_MS    4000u


// ------------------------------------------------------------
// 2. VARIABLES GLOBALES
// ------------------------------------------------------------

// Bandera modificada por el callback del temporizador.
//
// volatile indica que su contenido puede cambiar
// de forma asíncrona respecto al programa principal.
static volatile bool alarma_finalizada = false;

// Indica que la medición se encuentra activa.
static volatile bool medicion_activa = false;

// Indica que la interrupción GPIO detectó el botón.
static volatile bool boton_detectado = false;

// Guarda el instante de la pulsación.
//
// Se utiliza un entero de 64 bits porque el contador
// de tiempo del SDK proporciona microsegundos en 64 bits.
static volatile uint64_t tiempo_pulsacion_us = 0u;


// ------------------------------------------------------------
// 3. PROTOTIPOS DE FUNCIONES
// ------------------------------------------------------------

// Callback de una alarma programada.
static int64_t timer_callback(
    alarm_id_t id,
    void *user_data
);

// Callback de la interrupción GPIO.
static void gpio_callback(
    uint gpio,
    uint32_t events
);


// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 4.1 DECLARACIÓN DE VARIABLES LOCALES
    // --------------------------------------------------------

    // Identificador devuelto al programar una alarma.
    alarm_id_t alarma;

    // Instantes de inicio y final de la medición.
    absolute_time_t tiempo_inicio;
    absolute_time_t tiempo_final;

    // Tiempo transcurrido, expresado en microsegundos.
    int64_t tiempo_transcurrido_us;


    // --------------------------------------------------------
    // 4.2 INICIALIZACIÓN DE LA COMUNICACIÓN USB CDC
    // --------------------------------------------------------

    // Inicializa las interfaces estándar habilitadas
    // mediante CMake, incluyendo USB CDC.
    //
    // printf() transmitirá los mensajes por el puerto
    // COM virtual de la computadora.

    stdio_init_all();

    // Esperar hasta que el monitor serial establezca
    // la conexión USB CDC.
    //
    // tight_loop_contents() no genera la temporización;
    // únicamente indica que estamos en una espera activa.

    while (!stdio_usb_connected())
    {
        tight_loop_contents();
    }


    // --------------------------------------------------------
    // 4.3 CONFIGURACIÓN DEL BOTÓN
    // --------------------------------------------------------

    // Inicializar el GPIO.
    gpio_init(BUTTON_PIN);

    // Configurarlo como entrada digital.
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // Habilitar la resistencia pull-up interna.
    //
    // Botón liberado:
    //   GPIO = 1
    //
    // Botón presionado:
    //   GPIO = 0

    gpio_pull_up(BUTTON_PIN);

    // Inicialmente no se habilita la interrupción.
    //
    // Se habilitará únicamente cuando comience
    // la medición, después de las dos alarmas.

    gpio_set_irq_enabled_with_callback(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        false,
        &gpio_callback
    );


    // ========================================================
    // 5. PRIMERA ALARMA - 2 SEGUNDOS
    // ========================================================

    // Limpiar la bandera antes de iniciar la espera.
    alarma_finalizada = false;

    // Programar una alarma relativa de 2000 ms.
    //
    // add_alarm_in_ms() utiliza los mecanismos de alarma
    // proporcionados por el Pico SDK.
    //
    // Parámetros:
    //
    // TIEMPO_INICIAL_MS:
    //   Tiempo hasta el evento, en milisegundos.
    //
    // timer_callback:
    //   Función que se ejecutará al vencer la alarma.
    //
    // NULL:
    //   No se entregan datos adicionales al callback.
    //
    // false:
    //   No ejecutar anticipadamente el callback si
    //   el plazo ya ha transcurrido.

    alarma = add_alarm_in_ms(
        TIEMPO_INICIAL_MS,
        timer_callback,
        NULL,
        false
    );

    // Comprobar que el SDK aceptó la alarma.
    if (alarma <= 0)
    {
        printf("ERROR: No se pudo crear la primera alarma.\r\n");

        while (true)
        {
            tight_loop_contents();
        }
    }

    // Esperar hasta que el callback registre
    // la finalización de la alarma.
    //
    // El tiempo de 2 segundos no se calcula contando
    // las iteraciones de este ciclo.

    while (!alarma_finalizada)
    {
        tight_loop_contents();
    }


    // --------------------------------------------------------
    // 5.1 MENSAJE DE SISTEMA LISTO
    // --------------------------------------------------------

    printf("\r\n");
    printf("========================================\r\n");
    printf(" COMUNICACION USB CDC ESTABLECIDA\r\n");
    printf(" SISTEMA LISTO\r\n");
    printf("========================================\r\n");


    // ========================================================
    // 6. SEGUNDA ALARMA - 4 SEGUNDOS
    // ========================================================

    printf("La rutina comenzara en 4 segundos...\r\n");

    // Reiniciar la bandera de finalización.
    //
    // No se reinicia el contador físico TIMER.
    // Se solicita una nueva alarma relativa.

    alarma_finalizada = false;

    alarma = add_alarm_in_ms(
        TIEMPO_PREPARACION_MS,
        timer_callback,
        NULL,
        false
    );

    if (alarma <= 0)
    {
        printf("ERROR: No se pudo crear la segunda alarma.\r\n");

        while (true)
        {
            tight_loop_contents();
        }
    }

    // Esperar la segunda alarma de hardware.
    while (!alarma_finalizada)
    {
        tight_loop_contents();
    }


    // ========================================================
    // 7. PREPARACIÓN DE LA MEDICIÓN
    // ========================================================

    // Verificar que el botón esté liberado antes
    // de iniciar una nueva medición.
    //
    // Esto evita interpretar una pulsación que
    // comenzó durante la preparación.

    while (gpio_get(BUTTON_PIN) == BUTTON_PRESSED)
    {
        tight_loop_contents();
    }

    // Mostrar el mensaje antes de capturar el inicio.
    printf("\r\n");
    printf("========================================\r\n");
    printf(" INICIO DE MEDICION\r\n");
    printf(" PRESIONE EL BOTON PARA DETENER\r\n");
    printf("========================================\r\n");


    // --------------------------------------------------------
    // 7.1 INICIALIZAR LAS VARIABLES DE MEDICIÓN
    // --------------------------------------------------------

    boton_detectado = false;
    medicion_activa = false;
    tiempo_pulsacion_us = 0u;

    // Capturar el instante inicial mediante el
    // contador de tiempo del hardware.
    //
    // El TIMER funciona continuamente; esta lectura
    // establece la referencia de nuestra medición.

    tiempo_inicio = get_absolute_time();

    // Habilitar el registro del evento.
    medicion_activa = true;

    // Habilitar la interrupción GPIO.
    //
    // GPIO_IRQ_EDGE_FALL detecta el flanco de bajada:
    // transición de nivel lógico 1 a 0.

    gpio_set_irq_enabled(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        true
    );


    // ========================================================
    // 8. ESPERAR LA INTERRUPCIÓN DEL BOTÓN
    // ========================================================

    // El callback GPIO registrará el instante final.
    //
    // No se ejecutan cálculos ni retardos en este ciclo.

    while (!boton_detectado)
    {
        tight_loop_contents();
    }

    // Deshabilitar la interrupción después
    // de detectar la primera pulsación.

    gpio_set_irq_enabled(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        false
    );


    // --------------------------------------------------------
    // 8.1 RECUPERAR EL INSTANTE FINAL
    // --------------------------------------------------------

    // La interrupción ya escribió el instante final.
    // En esta práctica no se utiliza el segundo núcleo.

    tiempo_final = from_us_since_boot(
        tiempo_pulsacion_us
    );


    // --------------------------------------------------------
    // 8.2 OBTENER EL TIEMPO TRANSCURRIDO
    // --------------------------------------------------------

    // absolute_time_diff_us() es una función del SDK
    // que obtiene el intervalo entre las referencias
    // temporales registradas.
    //
    // No se incrementa ningún contador por software.
    //
    // El resultado se expresa en microsegundos.

    tiempo_transcurrido_us =
        absolute_time_diff_us(
            tiempo_inicio,
            tiempo_final
        );


    // ========================================================
    // 9. ENVIAR EL RESULTADO
    // ========================================================

    printf("\r\n");
    printf("========================================\r\n");
    printf(" MEDICION FINALIZADA\r\n");
    printf("========================================\r\n");

    printf(
        "Tiempo transcurrido: %" PRId64 " us\r\n",
        tiempo_transcurrido_us
    );

    printf("========================================\r\n");
    printf("Reinicie la tarjeta para repetir la prueba.\r\n");


    // ========================================================
    // 10. FIN DEL PROGRAMA
    // ========================================================

    // La práctica realiza una sola medición.
    // El programa permanece activo sin repetirla.

    while (true)
    {
        tight_loop_contents();
    }
}


// ============================================================
// 11. CALLBACK DE LA ALARMA DEL TIMER
// ============================================================

static int64_t timer_callback(
    alarm_id_t id,
    void *user_data
)
{
    // Evitar advertencias por parámetros no utilizados.
    (void)id;
    (void)user_data;

    // Informar al programa principal que
    // la alarma programada ha terminado.
    alarma_finalizada = true;

    // Retornar cero indica que la alarma
    // no debe programarse nuevamente.
    //
    // Es una alarma de una sola ejecución.

    return 0;
}


// ============================================================
// 12. CALLBACK DE LA INTERRUPCIÓN GPIO
// ============================================================

static void gpio_callback(
    uint gpio,
    uint32_t events
)
{
    // Comprobar que el evento pertenece a GP15
    // y corresponde a un flanco de bajada.

    if ((gpio == BUTTON_PIN) &&
        (events & GPIO_IRQ_EDGE_FALL))
    {
        // Registrar únicamente la primera pulsación
        // que ocurra mientras la medición esté activa.

        if (medicion_activa && !boton_detectado)
        {
            // Leer inmediatamente el contador de hardware.
            //
            // La captura ocurre durante la atención
            // de la interrupción, no directamente en
            // el instante eléctrico del flanco.

            tiempo_pulsacion_us = time_us_64();

            // Finalizar la medición.
            medicion_activa = false;

            // Notificar al programa principal.
            boton_detectado = true;
        }
    }
}
