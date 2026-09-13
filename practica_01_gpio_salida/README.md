# Práctica 01 — GPIO de salida

## Objetivos

- Configurar un GPIO como salida.
- Comprender HIGH y LOW como estados eléctricos.
- Comparar conexiones **source** y **sink**.
- Relacionar la conexión física con lógica activa en alto o activa en bajo.
- Introducir funciones sencillas sin separar todavía el programa en módulos.

## Punto de ingeniería

`gpio_put(pin, 1)` **no significa universalmente "encender la carga"**.

El estado activo depende de cómo esté conectada físicamente.
