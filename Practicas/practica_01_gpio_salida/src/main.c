
#include "pico/stdlib.h"

// ============================================================
// PRÁCTICA 01 - GPIO COMO SALIDAS DIGITALES
// ============================================================
//
// Objetivo:
//   Controlar dos semáforos correspondientes a dos vialidades
//   que se cruzan, mediante seis salidas digitales GPIO.
//
// Descripción:
//   Cada vialidad dispone de tres LED:
//      - Rojo
//      - Amarillo
//      - Verde
//
//   Cuando una vialidad tiene luz verde, la otra debe
//   permanecer en rojo.
//
//   Se considera un intervalo de seguridad en el que
//   ambas vialidades permanecen en rojo.
//
// Secuencia:
//   1. Vialidad A: Verde     | Vialidad B: Rojo
//   2. Vialidad A: Amarillo  | Vialidad B: Rojo
//   3. Vialidad A: Rojo      | Vialidad B: Rojo
//   4. Vialidad A: Rojo      | Vialidad B: Verde
//   5. Vialidad A: Rojo      | Vialidad B: Amarillo
//   6. Vialidad A: Rojo      | Vialidad B: Rojo
//
//   Al terminar, la secuencia vuelve a comenzar.
//
// Nota:
//   Ejemplo didáctico de control secuencial bloqueante.
//   No implementa detección de fallos ni seguridad vial real.
//
// ============================================================


// ------------------------------------------------------------
// 1. DEFINICIÓN DE PINES GPIO
// ------------------------------------------------------------

// Semáforo correspondiente a la vialidad A.
#define A_ROJO       10u
#define A_AMARILLO   11u
#define A_VERDE      12u

// Semáforo correspondiente a la vialidad B.
#define B_ROJO       13u
#define B_AMARILLO   14u
#define B_VERDE      15u


// ------------------------------------------------------------
// 2. DEFINICIÓN DE TIEMPOS
// ------------------------------------------------------------

// Todos los tiempos están expresados en milisegundos.
// 1000 ms equivalen a 1 segundo.

#define TIEMPO_VERDE_MS       5000u
#define TIEMPO_AMARILLO_MS    2000u
#define TIEMPO_SEGURIDAD_MS  1000u


// ------------------------------------------------------------
// 3. NIVELES LÓGICOS
// ------------------------------------------------------------

// Se consideran LED activos en nivel alto.
//
// GPIO = 1 -> LED encendido.
// GPIO = 0 -> LED apagado.
//
// Cada LED se conecta al GPIO mediante una resistencia
// limitadora de corriente, con su cátodo conectado a GND.

#define ENCENDIDO  1u
#define APAGADO    0u


// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // 4.1 INICIALIZACIÓN DE LOS GPIO
    // --------------------------------------------------------

    // Inicializar los seis pines para utilizarlos como GPIO.
    gpio_init(A_ROJO);
    gpio_init(A_AMARILLO);
    gpio_init(A_VERDE);

    gpio_init(B_ROJO);
    gpio_init(B_AMARILLO);
    gpio_init(B_VERDE);


    // --------------------------------------------------------
    // 4.2 ESTABLECER LOS NIVELES INICIALES
    // --------------------------------------------------------

    // Preparar el estado inicial de los pines.
    // Los LED rojos se activarán y los demás permanecerán
    // apagados cuando se habiliten las salidas.

    gpio_put(A_ROJO, ENCENDIDO);
    gpio_put(A_AMARILLO, APAGADO);
    gpio_put(A_VERDE, APAGADO);

    gpio_put(B_ROJO, ENCENDIDO);
    gpio_put(B_AMARILLO, APAGADO);
    gpio_put(B_VERDE, APAGADO);


    // --------------------------------------------------------
    // 4.3 CONFIGURACIÓN COMO SALIDAS DIGITALES
    // --------------------------------------------------------

    // GPIO_OUT configura el pin como salida.
    // A partir de este momento, el microcontrolador
    // puede controlar el nivel eléctrico de cada pin.

    gpio_set_dir(A_ROJO, GPIO_OUT);
    gpio_set_dir(A_AMARILLO, GPIO_OUT);
    gpio_set_dir(A_VERDE, GPIO_OUT);

    gpio_set_dir(B_ROJO, GPIO_OUT);
    gpio_set_dir(B_AMARILLO, GPIO_OUT);
    gpio_set_dir(B_VERDE, GPIO_OUT);


    // --------------------------------------------------------
    // 4.4 ESTADO INICIAL DE SEGURIDAD
    // --------------------------------------------------------

    // Ambas vialidades comienzan en rojo.
    // Se espera un segundo antes de iniciar la secuencia.

    sleep_ms(TIEMPO_SEGURIDAD_MS);


    // ========================================================
    // 5. CICLO PRINCIPAL
    // ========================================================

    // while(true) repite indefinidamente la secuencia.
    // sleep_ms() detiene temporalmente la ejecución
    // del programa principal durante el tiempo indicado.

    while (true)
    {
        // ====================================================
        // ESTADO 1: VIALIDAD A EN VERDE
        // ====================================================

        // A permite la circulación.
        // B permanece detenida con luz roja.

        gpio_put(A_ROJO, APAGADO);
        gpio_put(A_AMARILLO, APAGADO);
        gpio_put(A_VERDE, ENCENDIDO);

        gpio_put(B_ROJO, ENCENDIDO);
        gpio_put(B_AMARILLO, APAGADO);
        gpio_put(B_VERDE, APAGADO);

        // Mantener este estado durante 5 segundos.
        sleep_ms(TIEMPO_VERDE_MS);


        // ====================================================
        // ESTADO 2: VIALIDAD A EN AMARILLO
        // ====================================================

        // A pasa de verde a amarillo.
        // B continúa en rojo.

        gpio_put(A_ROJO, APAGADO);
        gpio_put(A_AMARILLO, ENCENDIDO);
        gpio_put(A_VERDE, APAGADO);

        gpio_put(B_ROJO, ENCENDIDO);
        gpio_put(B_AMARILLO, APAGADO);
        gpio_put(B_VERDE, APAGADO);

        // Mantener amarillo durante 2 segundos.
        sleep_ms(TIEMPO_AMARILLO_MS);


        // ====================================================
        // ESTADO 3: AMBAS VIALIDADES EN ROJO
        // ====================================================

        // A termina su fase de circulación.
        // B todavía no recibe autorización para avanzar.
        //
        // Este intervalo separa las fases de las vialidades.

        gpio_put(A_ROJO, ENCENDIDO);
        gpio_put(A_AMARILLO, APAGADO);
        gpio_put(A_VERDE, APAGADO);

        gpio_put(B_ROJO, ENCENDIDO);
        gpio_put(B_AMARILLO, APAGADO);
        gpio_put(B_VERDE, APAGADO);

        sleep_ms(TIEMPO_SEGURIDAD_MS);


        // ====================================================
        // ESTADO 4: VIALIDAD B EN VERDE
        // ====================================================

        // B permite la circulación.
        // A permanece detenida con luz roja.

        gpio_put(A_ROJO, ENCENDIDO);
        gpio_put(A_AMARILLO, APAGADO);
        gpio_put(A_VERDE, APAGADO);

        gpio_put(B_ROJO, APAGADO);
        gpio_put(B_AMARILLO, APAGADO);
        gpio_put(B_VERDE, ENCENDIDO);

        sleep_ms(TIEMPO_VERDE_MS);


        // ====================================================
        // ESTADO 5: VIALIDAD B EN AMARILLO
        // ====================================================

        // B pasa de verde a amarillo.
        // A continúa en rojo.

        gpio_put(A_ROJO, ENCENDIDO);
        gpio_put(A_AMARILLO, APAGADO);
        gpio_put(A_VERDE, APAGADO);

        gpio_put(B_ROJO, APAGADO);
        gpio_put(B_AMARILLO, ENCENDIDO);
        gpio_put(B_VERDE, APAGADO);

        sleep_ms(TIEMPO_AMARILLO_MS);


        // ====================================================
        // ESTADO 6: AMBAS VIALIDADES EN ROJO
        // ====================================================

        // Se detienen ambas vialidades antes de
        // comenzar un nuevo ciclo.

        gpio_put(A_ROJO, ENCENDIDO);
        gpio_put(A_AMARILLO, APAGADO);
        gpio_put(A_VERDE, APAGADO);

        gpio_put(B_ROJO, ENCENDIDO);
        gpio_put(B_AMARILLO, APAGADO);
        gpio_put(B_VERDE, APAGADO);

        sleep_ms(TIEMPO_SEGURIDAD_MS);

        // Regresar al estado 1.
    }
}
