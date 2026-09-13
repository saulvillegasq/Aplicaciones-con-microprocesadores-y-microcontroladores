# Arquitectura de firmware y buenas prácticas

## 1. Firmware no es software de PC en pequeño

En una PC es común trabajar sobre varias capas administradas por el sistema operativo:

```text
Aplicación
    ↓
Bibliotecas
    ↓
Sistema operativo
    ↓
Drivers
    ↓
Hardware
```

En firmware bare-metal la relación con el hardware es mucho más directa:

```text
Aplicación
    ↓
Drivers propios
    ↓
Pico SDK
    ↓
Registros
    ↓
Periféricos
    ↓
Mundo físico
```

En un MCU importan simultáneamente:

- software;
- hardware;
- tiempo;
- memoria;
- energía;
- estado inicial;
- comportamiento ante fallos.

---

## 2. Separación mínima recomendada

```text
src/main.c
```

Responsable únicamente de inicializar y ejecutar la aplicación.

```text
src/application.c
include/application.h
```

Contiene la lógica de la aplicación.

```text
drivers/
```

Contiene módulos que encapsulan periféricos o dispositivos.

Ejemplo:

```text
drivers/
├── led.c
├── led.h
├── button.c
└── button.h
```

---

## 3. `main.c` pequeño

Preferible:

```c
int main(void)
{
    hardware_init();
    application_init();

    while (true)
    {
        application_run();
    }
}
```

Evitar un `main.c` monolítico con inicialización, drivers, control, protocolo y
procesamiento mezclados.

---

## 4. Reglas del curso

1. Nombres descriptivos.
2. No usar números mágicos.
3. Definir explícitamente unidades (`ms`, `us`, `Hz`, `V`).
4. Inicializar el hardware de forma conocida.
5. Mantener estados de salida seguros durante el arranque.
6. Evitar trabajo pesado dentro de ISR.
7. Distinguir código bloqueante y no bloqueante.
8. Preferir memoria estática cuando el tamaño máximo es conocido.
9. Manejar errores de periféricos y comunicaciones.
10. Documentar las decisiones físicas: polaridad, pull-up, frecuencia, rango ADC, etc.

---
