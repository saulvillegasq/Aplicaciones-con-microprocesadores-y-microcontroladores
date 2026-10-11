# Práctica 02 — GPIO de entrada y polling

## Objetivos

- Configurar entradas digitales.
- Analizar entradas flotantes.
- Usar pull-up y pull-down.
- Comprender señales activas en bajo.
- Implementar polling.
- Observar rebote mecánico.
- Introducir una primera solución bloqueante de debounce.

## Punto de ingeniería

Una entrada digital puede tener un comportamiento físico no ideal aunque el código sólo vea `0` y `1`.
