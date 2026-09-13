# Configuración de VS Code, Pico SDK, CMake y toolchain

Esta guía describe la instalación y configuración del entorno de desarrollo utilizado en el curso para trabajar con **Raspberry Pi Pico / RP2040** mediante el **Raspberry Pi Pico C/C++ SDK**.

El objetivo no es únicamente instalar un entorno gráfico. El estudiante debe reconocer qué herramienta participa en cada etapa:

```text
Código fuente
    ↓
CMake
    ↓
Ninja
    ↓
Compilador cruzado ARM
    ↓
Enlazador
    ↓
ELF / BIN / HEX / UF2
    ↓
Programación
    ↓
Raspberry Pi Pico
```

> **PUNTO CLAVE:** durante el curso se utilizará un entorno **reproducible**. No se recomienda actualizar el Pico SDK o el toolchain a mitad del semestre salvo indicación del profesor.

---

## 1. Configuración de referencia del curso

Para las prácticas se utilizará como referencia:

| Componente | Versión de referencia | Criterio |
|---|---:|---|
| Raspberry Pi Pico SDK | **2.3.1** | Versión fijada para el curso |
| Visual Studio Code | **versión estable actual; mínimo 1.105.1** | Mínimo exigido por la extensión oficial Pico |
| CMake | **3.13 o superior** | Mínimo requerido por Pico SDK |
| Ninja | **1.13.x o estable equivalente** | Generador recomendado |
| Arm GNU Toolchain | **15.3.Rel1 / GCC 15.3.x** | Toolchain `arm-none-eabi` compatible con SDK 2.3.1 |
| Git | **2.28 o superior** | Requisito mínimo indicado por la extensión en macOS/Linux |
| Python | **3.10 o superior** | Requisito de la extensión en Linux |

**Para las prácticas no se trabajará sobre la rama `develop (versión más reciente en desarrollo)` del Pico SDK.**

La versión del SDK se fija explícitamente en:

```text
2.3.1
```

Esto evita que dos estudiantes compilen el mismo proyecto con APIs o dependencias diferentes.

---

## 2. Sistemas operativos soportados

La extensión oficial **Raspberry Pi Pico** para VS Code soporta:

- **Windows 10/11**
- **macOS Sonoma 14 o posterior**
- **Linux x64**
- **Linux arm64**
- **Raspberry Pi OS de 64 bits**

### Windows

Para el laboratorio se recomienda:

```text
Windows 10/11 de 64 bits
```

En equipos Intel/AMD se deben seleccionar paquetes:

```text
x64
x86_64
```

### macOS

Para equipos Apple Silicon:

```text
arm64
Apple Silicon
Universal
```

Para equipos Intel:

```text
x86_64
Intel
Universal
```

Antes de comenzar:

```bash
xcode-select --install
```

### Linux

Se recomienda una distribución de 64 bits actual, por ejemplo:

```text
Ubuntu 22.04 LTS o posterior
```

### Raspberry Pi OS

Utilizar:

```text
Raspberry Pi OS de 64 bits
```

> **PUNTO CLAVE:** antes de descargar una herramienta, comprobar siempre la **arquitectura del sistema operativo anfitrión**: `x64/x86_64`, `arm64/aarch64` o Apple Silicon.

---

## 3. Dos formas de preparar el entorno

Existen dos rutas válidas.

### Opción A — Instalación administrada por la extensión oficial

La extensión **Raspberry Pi Pico** puede administrar:

- Pico SDK;
- toolchain;
- CMake;
- Ninja;
- OpenOCD;
- herramientas de compilación y depuración.

Es la forma más cómoda.

### Opción B — Instalación manual

En este curso se explicará también la instalación manual para que el estudiante entienda la relación entre:

```text
VS Code
CMake
Ninja
arm-none-eabi-gcc
Pico SDK
```

> **PUNTO CLAVE:** aunque la extensión pueda automatizar el entorno, el estudiante debe saber compilar desde una terminal sin depender del botón **Build** del IDE.

> **EVITAR:** mantener simultáneamente varias instalaciones distintas del Pico SDK/toolchain sin saber cuál está usando VS Code o CMake. Esto es una causa frecuente de errores de configuración.

---

# 4. Visual Studio Code

Descarga oficial:

```text
https://code.visualstudio.com/Download
```

Instalar la versión correspondiente al sistema operativo.

### Windows Intel/AMD

Seleccionar:

```text
Windows x64
```

### macOS Apple Silicon

Seleccionar:

```text
Apple Silicon
```

o:

```text
Universal
```

### macOS Intel

Seleccionar:

```text
Intel
```

o:

```text
Universal
```

### Linux

Seleccionar el paquete adecuado:

```text
.deb
.rpm
tar.gz
```

Verificar desde terminal:

```powershell
code --version
```

La extensión oficial de Raspberry Pi Pico requiere:

```text
VS Code >= 1.105.1
```

> **PUNTO CLAVE:** **Visual Studio Code** y **Visual Studio** son productos diferentes. Para este curso se utiliza **Visual Studio Code**.

---

# 5. Extensiones necesarias de VS Code

Abrir VS Code y pulsar:

```text
Ctrl + Shift + X
```

En macOS:

```text
Cmd + Shift + X
```

## 5.1 Raspberry Pi Pico — necesaria para el curso

Buscar:

```text
Raspberry Pi Pico
```

Publisher:

```text
Raspberry Pi
```

Marketplace oficial:

```text
https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico
```

Esta extensión proporciona, entre otras funciones:

- creación e importación de proyectos Pico;
- selección de versión del Pico SDK;
- configuración automática de CMake;
- soporte para Ninja;
- compilación;
- depuración mediante OpenOCD;
- documentación del SDK dentro de VS Code;
- selección/configuración de herramientas.

> **PUNTO CLAVE:** verificar que el publisher sea **Raspberry Pi**. No instalar una extensión de nombre parecido publicada por un tercero.

---

## 5.2 C/C++ — necesaria/recomendada para edición de C

Buscar:

```text
C/C++
```

Publisher:

```text
Microsoft
```

Identificador:

```text
ms-vscode.cpptools
```

Marketplace:

```text
https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
```

Proporciona:

- IntelliSense;
- navegación por símbolos;
- resaltado semántico;
- detección de errores;
- integración con depuración C/C++.

---

## 5.3 CMake Tools — recomendada para el curso

Buscar:

```text
CMake Tools
```

Publisher:

```text
Microsoft
```

Identificador:

```text
ms-vscode.cmake-tools
```

Marketplace:

```text
https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
```

La extensión oficial de Raspberry Pi Pico puede integrarse con **CMake Tools**, especialmente en proyectos con varios ejecutables o configuraciones CMake más complejas.

> **PUNTO CLAVE:** **CMake Tools no sustituye a CMake**. Es una interfaz de VS Code para trabajar con una instalación real de CMake.

Si CMake Tools solicita un kit para un proyecto Pico, seleccionar:

```text
Pico
```

cuando se encuentre disponible.

---

## 5.4 Extensiones opcionales

No son necesarias para realizar las prácticas.

### GitLens

Útil para inspeccionar historial Git:

```text
GitLens
```

### Markdown All in One

Útil para editar los reportes y archivos `.md`:

```text
Markdown All in One
```

### Hex Editor

Puede ser útil para inspeccionar archivos binarios:

```text
Hex Editor
Publisher: Microsoft
```

> **PUNTO CLAVE:** no se instalarán extensiones adicionales “porque sí”. Cada extensión agrega comportamiento al entorno y puede introducir conflictos. Para el curso bastan principalmente **Raspberry Pi Pico + C/C++ + CMake Tools**.

---

# 6. Git

Página oficial:

```text
https://git-scm.com/downloads
```

Se utiliza para:

- descargar el Pico SDK;
- descargar ejemplos;
- administrar versiones del repositorio de prácticas;
- trabajar con commits y ramas.

## Windows

Descargar:

```text
Git for Windows x64
```

en equipos Intel/AMD de 64 bits.

## macOS

Puede obtenerse mediante las Command Line Tools:

```bash
xcode-select --install
```

## Ubuntu/Debian

```bash
sudo apt update
sudo apt install git
```

Verificar:

```powershell
git --version
```

Versión recomendada:

```text
Git >= 2.28
```

---

# 7. CMake

Descarga oficial:

```text
https://cmake.org/download/
```

El Pico SDK requiere como mínimo:

```text
CMake 3.13
```

## Windows

Descargar el instalador para:

```text
Windows x86_64
```

Durante la instalación, habilitar CMake en:

```text
PATH
```

## macOS

Puede instalarse mediante el paquete oficial o:

```bash
brew install cmake
```

## Ubuntu/Debian

```bash
sudo apt update
sudo apt install cmake
```

Verificar:

```powershell
cmake --version
```

> **PUNTO CLAVE:** **CMake no compila directamente el firmware.** CMake configura/genera el sistema de construcción.

---

# 8. Ninja

Ninja será el ejecutor de construcción recomendado.

Descarga oficial:

```text
https://github.com/ninja-build/ninja/releases
```

Versión recomendada:

```text
Ninja 1.13.x
```

## Windows

Descargar el binario para Windows y comprobar que `ninja.exe` esté disponible mediante:

```text
PATH
```

## macOS

```bash
brew install ninja
```

## Ubuntu/Debian

```bash
sudo apt install ninja-build
```

Verificar:

```powershell
ninja --version
```

> **PUNTO CLAVE:**
>
> ```text
> CMake → genera/configura
> Ninja → ejecuta la construcción
> ```

---

# 9. Arm GNU Toolchain

La Raspberry Pi Pico utiliza el **RP2040**, cuyo procesador principal es un **Arm Cortex-M0+**.

Por ello se necesita un compilador cruzado:

```text
arm-none-eabi-gcc
```

Descarga oficial:

```text
https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
```

Seleccionar la variante:

```text
arm-none-eabi
```

porque es el toolchain para desarrollo embebido **bare-metal**.

Versión de referencia:

```text
Arm GNU Toolchain 15.3.Rel1
```

El Pico SDK 2.3.1 incluye soporte para GCC 15.3.x.

## Windows

En equipos Intel/AMD seleccionar el paquete para:

```text
Windows x86_64
```

Después de instalar, comprobar que la carpeta `bin` del toolchain esté en `PATH`.

## Linux

La alternativa proporcionada por Ubuntu/Debian es:

```bash
sudo apt install gcc-arm-none-eabi
sudo apt install libnewlib-arm-none-eabi
sudo apt install libstdc++-arm-none-eabi-newlib
```

## Verificación

```powershell
arm-none-eabi-gcc --version
```

> **PUNTO CLAVE:** no confundir:
>
> ```text
> gcc
> ```
>
> con:
>
> ```text
> arm-none-eabi-gcc
> ```
>
> `gcc` compila normalmente para la computadora anfitriona.  
> `arm-none-eabi-gcc` genera código para el microcontrolador ARM.

---

# 10. Python

Python es utilizado por herramientas auxiliares del ecosistema Pico y por la extensión en algunos sistemas.

Descarga oficial:

```text
https://www.python.org/downloads/
```

Para Linux, la extensión oficial de Raspberry Pi Pico requiere:

```text
Python >= 3.10
```

Verificar:

```powershell
python --version
```

o:

```bash
python3 --version
```

Ubuntu/Debian:

```bash
sudo apt install python3
```

---

# 11. Descargar el Raspberry Pi Pico SDK

Repositorio oficial:

```text
https://github.com/raspberrypi/pico-sdk
```

Versión utilizada en el curso:

```text
2.3.1
```

## Windows PowerShell

Crear una carpeta para desarrollo:

```powershell
New-Item -ItemType Directory -Force C:\dev
cd C:\dev
```

Clonar **la versión fijada**:

```powershell
git clone --branch 2.3.1 --depth 1 https://github.com/raspberrypi/pico-sdk.git
```

Entrar al repositorio:

```powershell
cd pico-sdk
```

Descargar los submódulos:

```powershell
git submodule update --init --recursive
```

## Linux/macOS

Ejemplo:

```bash
mkdir -p ~/dev
cd ~/dev

git clone --branch 2.3.1 --depth 1 https://github.com/raspberrypi/pico-sdk.git

cd pico-sdk
git submodule update --init --recursive
```

Comprobar la versión:

```bash
git describe --tags
```

> **PUNTO CLAVE:** no usar simplemente la rama `develop`.  
> Para el curso todos deben trabajar sobre la **misma versión del SDK: 2.3.1**.

---

# 12. Definir `PICO_SDK_PATH`

Esta variable indica a CMake dónde se encuentra el SDK.

## Windows — sesión actual

```powershell
$env:PICO_SDK_PATH = "C:\dev\pico-sdk"
```

Comprobar:

```powershell
$env:PICO_SDK_PATH
```

## Windows — variable persistente

```powershell
[Environment]::SetEnvironmentVariable(
    "PICO_SDK_PATH",
    "C:\dev\pico-sdk",
    "User"
)
```

Después:

**cerrar y volver a abrir VS Code y PowerShell**.

## Linux/macOS — sesión actual

```bash
export PICO_SDK_PATH="$HOME/dev/pico-sdk"
```

Para hacerlo persistente, añadir al archivo de shell correspondiente, por ejemplo:

```bash
echo 'export PICO_SDK_PATH="$HOME/dev/pico-sdk"' >> ~/.bashrc
```

o, para Zsh:

```bash
echo 'export PICO_SDK_PATH="$HOME/dev/pico-sdk"' >> ~/.zshrc
```

> **PUNTO CLAVE:** `PICO_SDK_PATH` debe apuntar a la **raíz del SDK**, no a `src/`, `external/` ni a otra subcarpeta.

---

# 13. Copiar `pico_sdk_import.cmake`

El SDK incluye el archivo:

```text
<PICO_SDK_PATH>/external/pico_sdk_import.cmake
```

Desde la raíz del repositorio de prácticas, en Windows:

```powershell
Copy-Item `
    "$env:PICO_SDK_PATH\external\pico_sdk_import.cmake" `
    ".\pico_sdk_import.cmake" `
    -Force
```

En Linux/macOS:

```bash
cp "$PICO_SDK_PATH/external/pico_sdk_import.cmake" ./pico_sdk_import.cmake
```

Cada práctica utiliza:

```cmake
include(../pico_sdk_import.cmake)
```

antes de:

```cmake
project(...)
```

---

# 14. Abrir el repositorio en VS Code

Desde la raíz:

```powershell
code .
```

Comprobar en la terminal integrada:

```powershell
git --version
cmake --version
ninja --version
arm-none-eabi-gcc --version
code --version
```

Y:

```powershell
$env:PICO_SDK_PATH
```

En Linux/macOS:

```bash
echo "$PICO_SDK_PATH"
```

---

# 15. Configurar la primera práctica

Desde la raíz del repositorio:

```powershell
cmake `
    -S practica_01_gpio `
    -B practica_01_gpio/build `
    -G Ninja `
    -DPICO_BOARD=pico
```

Esto significa:

```text
-S → carpeta del código fuente
-B → carpeta donde CMake genera la construcción
-G → generador utilizado
-DPICO_BOARD=pico → tarjeta objetivo
```

Para Raspberry Pi Pico W:

```powershell
cmake `
    -S practica_01_gpio `
    -B practica_01_gpio/build `
    -G Ninja `
    -DPICO_BOARD=pico_w
```

> **PUNTO CLAVE:** la carpeta `build/` contiene archivos generados. **No debe utilizarse para guardar código fuente.**

---

# 16. Compilar

Después de configurar:

```powershell
cmake --build practica_01_gpio/build
```

También puede usarse:

```powershell
cmake --build practica_01_gpio/build --target practica_01_gpio
```

Entre los archivos generados aparecerán normalmente formatos como:

```text
practica_01_gpio.elf
practica_01_gpio.bin
practica_01_gpio.hex
practica_01_gpio.uf2
```

El archivo utilizado para programación mediante BOOTSEL será:

```text
practica_01_gpio.uf2
```

> **PUNTO CLAVE:** después de modificar solamente un `.c` o `.h`, normalmente basta con volver a ejecutar:
>
> ```powershell
> cmake --build practica_01_gpio/build
> ```

No es necesario borrar/configurar todo cada vez.

---

# 17. Programar mediante BOOTSEL / UF2

1. Desconectar la Raspberry Pi Pico.
2. Mantener presionado **BOOTSEL**.
3. Conectar el cable USB.
4. Soltar **BOOTSEL**.
5. La Pico aparecerá como una unidad de almacenamiento.
6. Copiar el archivo:

```text
practica_01_gpio.uf2
```

a esa unidad.

La tarjeta se reiniciará y ejecutará el firmware.

> **PUNTO CLAVE:** BOOTSEL/UF2 permite programar la tarjeta sin utilizar todavía un depurador SWD.

---

# 18. Limpiar una construcción

Si se cambia de versión del SDK, de toolchain, de tarjeta o aparecen problemas de caché de CMake, eliminar `build/`.

Windows PowerShell:

```powershell
Remove-Item -Recurse -Force practica_01_gpio/build
```

Después:

```powershell
cmake `
    -S practica_01_gpio `
    -B practica_01_gpio/build `
    -G Ninja `
    -DPICO_BOARD=pico

cmake --build practica_01_gpio/build
```

> **PUNTO CLAVE:** al cambiar de versión del Pico SDK es recomendable **borrar y recrear la carpeta `build/`**.

---

# 19. Documentación que debe utilizarse durante el curso

No se recomienda resolver las prácticas copiando ejemplos aleatorios de blogs sin comprobar la documentación del fabricante.

## Documentación principal de Raspberry Pi

### Raspberry Pi Pico C/C++ SDK

```text
https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html
```

### Documentación de la API del Pico SDK

```text
https://www.raspberrypi.com/documentation/pico-sdk/
```

### Repositorio oficial Pico SDK

```text
https://github.com/raspberrypi/pico-sdk
```

### Ejemplos oficiales

```text
https://github.com/raspberrypi/pico-examples
```

### Getting Started with Pico

```text
https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf
```

---

## Documentación del hardware

### RP2040 Datasheet

```text
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
```

Aquí se encuentra la descripción detallada de:

- GPIO;
- interrupciones;
- timers;
- PWM;
- ADC;
- UART;
- SPI;
- I²C;
- PIO;
- registros;
- arquitectura interna del RP2040.

### Raspberry Pi Pico Datasheet

```text
https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf
```

Este documento describe principalmente:

- pinout de la tarjeta;
- alimentación;
- conexiones del RP2040;
- memoria Flash;
- USB;
- BOOTSEL;
- características eléctricas de la placa.

> **PUNTO CLAVE:**
>
> - **Pico SDK Documentation** → consultar **cómo utilizar una API**.
> - **RP2040 Datasheet** → consultar **cómo funciona el periférico y sus registros**.
> - **Pico Datasheet** → consultar **cómo está construida y conectada la tarjeta**.

---

# 20. Documentación de las herramientas

## CMake

```text
https://cmake.org/documentation/
```

## Ninja

```text
https://ninja-build.org/manual.html
```

## Arm GNU Toolchain

```text
https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
```

## Visual Studio Code

```text
https://code.visualstudio.com/docs
```

## Git

```text
https://git-scm.com/doc
```

---

# 21. Flujo completo que debe reconocer el estudiante

```text
Código fuente
    ↓
Preprocesado
    ↓
Compilación
    ↓
Ensamblado
    ↓
Objetos
    ↓
Enlazado
    ↓
ELF
    ↓
BIN / HEX / UF2
    ↓
Programación
    ↓
Reset
    ↓
Ejecución en el MCU
```

## ¿Dónde interviene cada herramienta?

| Herramienta | Función |
|---|---|
| **VS Code** | Editor e interfaz de trabajo |
| **Raspberry Pi Pico extension** | Integración de proyecto, SDK, build y debug |
| **C/C++ extension** | IntelliSense y soporte de edición C/C++ |
| **CMake Tools** | Integración de CMake con VS Code |
| **Git** | Control de versiones y descarga de repositorios |
| **CMake** | Configura el sistema de construcción |
| **Ninja** | Ejecuta las reglas generadas |
| **arm-none-eabi-gcc** | Compila código para ARM bare-metal |
| **Linker** | Une objetos y bibliotecas |
| **Pico SDK** | API, bibliotecas y soporte de hardware |
| **OpenOCD** | Depuración/programación mediante interfaz de debug |
| **UF2** | Formato utilizado para programación sencilla mediante BOOTSEL |

---

# 22. Ejemplo mínimo de `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.13)

include(../pico_sdk_import.cmake)

project(mi_practica C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(mi_practica
    src/main.c
)

target_link_libraries(mi_practica
    pico_stdlib
)

pico_add_extra_outputs(mi_practica)
```

---

# 23. Ejemplo mínimo de `main.c`

```c
#include "pico/stdlib.h"

int main(void)
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);

        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
```

Este programa solamente verifica:

```text
Editor
  ↓
CMake
  ↓
Toolchain
  ↓
Pico SDK
  ↓
Compilación
  ↓
UF2
  ↓
Pico
```

**No representa todavía la arquitectura de software que se utilizará en las prácticas.**

Las prácticas posteriores separarán:

```text
main
application
drivers
hardware
```

---

# 24. Lista de comprobación antes de iniciar la primera práctica

El estudiante debe poder ejecutar sin errores:

```powershell
git --version
cmake --version
ninja --version
arm-none-eabi-gcc --version
code --version
```

Debe existir:

```text
PICO_SDK_PATH
```

y debe apuntar a la versión:

```text
pico-sdk 2.3.1
```

En VS Code deben estar instaladas:

- **Raspberry Pi Pico — Raspberry Pi**
- **C/C++ — Microsoft**
- **CMake Tools — Microsoft**

Finalmente debe poder ejecutar:

```powershell
cmake `
    -S practica_01_gpio `
    -B practica_01_gpio/build `
    -G Ninja `
    -DPICO_BOARD=pico

cmake --build practica_01_gpio/build
```

y obtener:

```text
practica_01_gpio.uf2
```

Si esto funciona, **el entorno del curso está correctamente configurado**.
