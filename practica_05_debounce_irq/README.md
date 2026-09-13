# Práctica 05 — Rebote e interrupciones

## Objetivos

- Observar que una sola pulsación puede producir varias IRQ.
- Demostrar por qué `sleep_ms()` dentro de una ISR es una mala práctica.
- Introducir ventana temporal de debounce.
- Validar el estado después de un intervalo.
- Preparar el bloque de temporizadores.

## Flujo deseado

```text
flanco
  ↓
ISR mínima
  ↓
registrar evento
  ↓
esperar ventana temporal sin bloquear
  ↓
validar
  ↓
procesar
```
