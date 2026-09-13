# Compilación y programación

Esta guía describe el procedimiento utilizado en el curso para **configurar, compilar y programar** las prácticas de Raspberry Pi Pico.

El procedimiento se apoya en:

- **Visual Studio Code**;
- la extensión oficial **Raspberry Pi Pico**, publicada por **Raspberry Pi**;
- las herramientas descargadas y administradas por dicha extensión;
- un único script **`build.ps1`**, ubicado en la raíz del repositorio.

> **PUNTO CLAVE:** los alumnos no necesitan instalar manualmente una segunda copia de CMake, Ninja, Python, `arm-none-eabi-gcc`, Picotool o PIOASM si la extensión oficial ya instaló esas herramientas dentro de `.pico-sdk`.

---

# 1. Flujo completo

```text
Código .c / .h
      ↓
Preprocesado
      ↓
Compilación
      ↓
Ensamblado
      ↓
Objetos .o
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
Ejecución en el RP2040
```

El alumno debe reconocer que el comando:

```powershell
.\build.ps1 practica_01_gpio
```

automatiza varias herramientas:

```text
build.ps1
    ↓
CMake
    ↓
Ninja
    ↓
Arm GNU Toolchain
    ↓
Linker
    ↓
ELF
    ↓
UF2
```

**CMake no es el compilador.** CMake configura la construcción; Ninja ejecuta las reglas y el toolchain ARM genera el código para el RP2040.

---

# 2. Primera instalación en una computadora

## 2.1 Instalar Visual Studio Code

Descargar e instalar Visual Studio Code.

Después abrir:

```text
Ctrl + Shift + X
```

e instalar:

```text
Raspberry Pi Pico
Publisher: Raspberry Pi
```

> **IMPORTANTE:** comprobar que el publisher sea **Raspberry Pi**.

## 2.2 Inicializar la extensión por primera vez

Instalar la extensión no siempre descarga inmediatamente todas las herramientas.

Abrir:

```text
Ctrl + Shift + P
```

y utilizar:

```text
Raspberry Pi Pico: New C/C++ Project
```

o:

```text
Raspberry Pi Pico: Import Project
```

Para el curso utilizar:

```text
Board       = Raspberry Pi Pico
PICO_BOARD  = pico
Pico SDK    = 2.3.1
```

La extensión descargará su entorno de desarrollo.

---

# 3. Comprobar `.pico-sdk`

Después de inicializar un proyecto debe existir:

```text
C:\Users\<usuario>\.pico-sdk
```

Comprobar desde PowerShell:

```powershell
Test-Path "$env:USERPROFILE\.pico-sdk"
```

Resultado esperado:

```text
True
```

También puede inspeccionarse:

```powershell
Get-ChildItem "$env:USERPROFILE\.pico-sdk"
```

Allí pueden aparecer carpetas como:

```text
sdk
toolchain
cmake
ninja
python
picotool
tools
openocd
```

> **PUNTO CLAVE:** `.pico-sdk` es el entorno administrado por la extensión. No debe confundirse con la carpeta donde se encuentra instalada la propia extensión de VS Code.

---

# 4. Versiones utilizadas en el curso

```text
Pico SDK       2.3.1
Toolchain ARM  15_2_Rel1
Picotool       2.3.1
Board          pico
MCU            RP2040
```

CMake, Ninja y Python son los instalados por la extensión.

**No actualizar individualmente estas herramientas durante el curso sin indicación del profesor.**

---

# 5. Ubicación del script

La raíz del repositorio contiene:

```text
microcontroladores_raspberry_pico/
│
├── build.ps1
├── pico_sdk_import.cmake
├── README.md
├── docs/
│
├── practica_01_gpio/
├── practica_02_interrupciones/
├── practica_03_temporizadores/
├── practica_04_adc/
└── practica_05_comunicacion/
```

Existe **un solo `build.ps1`** para todas las prácticas.

---

# 6. Abrir el repositorio

Ejemplo:

```powershell
cd C:\ruta\microcontroladores_raspberry_pico
code .
```

La terminal debe estar situada en la carpeta donde se encuentra:

```text
build.ps1
```

---

# 7. Si Windows bloquea el script

Permitir scripts **únicamente durante la sesión actual**:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
```

No es necesario modificar permanentemente la política de Windows.

---

# 8. Primera prueba del entorno

Ejecutar:

```powershell
.\build.ps1 practica_01_gpio -Clean -VerboseTools
```

El script debe mostrar rutas similares a:

```text
Pico SDK       : C:\Users\<usuario>\.pico-sdk\sdk\2.3.1
Toolchain ARM  : C:\Users\<usuario>\.pico-sdk\toolchain\15_2_Rel1
GCC ARM        : ...\arm-none-eabi-gcc.exe
CMake          : ...\cmake.exe
Ninja          : ...\ninja.exe
Python         : ...\python.exe
Picotool DIR   : ...
PIOASM DIR     : ...
pico-vscode    : ...\pico-vscode.cmake
```

Si estas rutas aparecen, el entorno fue detectado correctamente.

---

# 9. Uso cotidiano

## Compilar

```powershell
.\build.ps1 practica_01_gpio
```

## Limpiar y recompilar

```powershell
.\build.ps1 practica_01_gpio -Clean
```

## Mostrar las herramientas detectadas

```powershell
.\build.ps1 practica_01_gpio -VerboseTools
```

## Compilar otra práctica

```powershell
.\build.ps1 practica_02_interrupciones
.\build.ps1 practica_03_temporizadores
.\build.ps1 practica_04_adc
.\build.ps1 practica_05_comunicacion
```

---

# 10. ¿Qué cambió en `build.ps1` y por qué?

La primera versión localizaba correctamente el compilador ARM, CMake, Ninja y Python, pero todavía podía ocurrir que CMake no encontrara algunas **herramientas que se ejecutan en Windows**.

Por eso el script ahora distingue dos grupos.

## 10.1 Herramientas para el microcontrolador

```text
arm-none-eabi-gcc
```

genera código para:

```text
RP2040
```

Es un **cross-compiler**.

## 10.2 Herramientas que se ejecutan en la PC

```text
CMake
Ninja
Python
Picotool
PIOASM
```

se ejecutan en Windows durante la construcción.

Esto explica un error aparentemente contradictorio:

```text
arm-none-eabi-gcc encontrado
```

pero después:

```text
No CMAKE_C_COMPILER could be found
No CMAKE_CXX_COMPILER could be found
```

Ese mensaje puede aparecer cuando CMake intenta **compilar Picotool desde código fuente para Windows**, no cuando está compilando el firmware para el RP2040.

La corrección consiste en indicarle dónde está el **Picotool precompilado** que ya instaló la extensión.

---

# 11. Cambios principales del script

## 11.1 Ya no contiene el nombre del usuario

No utiliza:

```text
C:\Users\PLATA
```

sino:

```powershell
$env:USERPROFILE
```

Por tanto funciona para:

```text
C:\Users\Alumno1
C:\Users\Alumno2
C:\Users\Profesor
```

sin modificar el archivo.

## 11.2 Usa versiones fijas del curso

Al comienzo del script aparecen:

```powershell
$SdkVersion       = "2.3.1"
$ToolchainVersion = "15_2_Rel1"
$PicotoolVersion  = "2.3.1"
```

Los alumnos **no deben cambiar estos valores** salvo indicación del profesor.

## 11.3 Detecta Python administrado por la extensión

Por ejemplo:

```text
C:\Users\<usuario>\.pico-sdk\python\3.13.7\python.exe
```

Por tanto no es necesario instalar otra copia de Python únicamente para compilar las prácticas.

## 11.4 Detecta Picotool y PIOASM

El script busca sus archivos de configuración:

```text
picotoolConfig.cmake
pioasmConfig.cmake
```

y pasa las carpetas encontradas directamente a CMake.

Esto evita que el SDK intente compilar Picotool desde código fuente.

## 11.5 No modifica permanentemente el PATH

El script agrega temporalmente CMake, Ninja, Python y GCC ARM al:

```text
PATH
```

**únicamente durante su ejecución**.

Al cerrar PowerShell no se altera la configuración global de Windows.

## 11.6 Utiliza `pico-vscode.cmake`

Los `CMakeLists.txt` de las prácticas incluyen:

```cmake
set(picoVscode "${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake")

if(EXISTS "${picoVscode}")
    include("${picoVscode}")
endif()
```

Este archivo forma parte del entorno que instala la extensión oficial y permite que el proyecto use la misma organización de SDK, toolchain, Picotool y PIOASM que un proyecto creado desde VS Code.

---

# 12. Qué hace internamente `build.ps1`

```text
Recibe nombre de práctica
        ↓
Busca C:\Users\<usuario>\.pico-sdk
        ↓
Comprueba SDK 2.3.1
        ↓
Comprueba toolchain 15_2_Rel1
        ↓
Localiza CMake
        ↓
Localiza Ninja
        ↓
Localiza Python
        ↓
Localiza Picotool
        ↓
Localiza PIOASM
        ↓
Prepara PATH temporal
        ↓
Copia pico_sdk_import.cmake
        ↓
Configura CMake
        ↓
Compila mediante Ninja
        ↓
Busca el .uf2
```

---

# 13. Comando CMake que automatiza el script

Conceptualmente ejecuta:

```powershell
cmake `
    -S practica_01_gpio `
    -B practica_01_gpio/build `
    -G Ninja `
    -DPICO_BOARD=pico
```

además de pasar las rutas del SDK, toolchain, Python, Picotool y PIOASM.

Después:

```powershell
cmake --build practica_01_gpio/build
```

El alumno debe comprender estos comandos aunque durante el curso utilice `build.ps1`.

---

# 14. Carpeta `build/`

```text
practica_01_gpio/
├── CMakeLists.txt
├── src/
├── include/
├── drivers/
└── build/
```

`build/` contiene **archivos generados**.

Puede contener:

```text
CMakeCache.txt
CMakeFiles/
*.o
*.elf
*.bin
*.hex
*.uf2
```

**Nunca debe utilizarse para guardar código fuente.**

Por esta razón debe estar en:

```text
.gitignore
```

---

# 15. Cuándo utilizar `-Clean`

Usar:

```powershell
.\build.ps1 practica_01_gpio -Clean
```

cuando:

- sea la primera compilación después de cambiar el script;
- cambie el `CMakeLists.txt`;
- cambie la tarjeta;
- cambie el SDK o toolchain;
- aparezcan problemas de caché;
- se quiera comprobar una compilación completamente reproducible.

Después de modificar únicamente un `.c` o `.h`, normalmente basta con:

```powershell
.\build.ps1 practica_01_gpio
```

---

# 16. Artefactos

Después de una construcción correcta pueden aparecer:

```text
practica_01_gpio.elf
practica_01_gpio.bin
practica_01_gpio.hex
practica_01_gpio.uf2
```

El script muestra automáticamente la ruta del `.uf2`.

---

# 17. Compilar no es programar

## Compilar

```text
Código
  ↓
Toolchain
  ↓
UF2
```

## Programar

```text
UF2
 ↓
Flash del microcontrolador
```

Por tanto:

```text
COMPILAR ≠ PROGRAMAR
```

---

# 18. Programar mediante BOOTSEL

1. Desconectar la Pico.
2. Mantener presionado **BOOTSEL**.
3. Conectar USB.
4. Soltar **BOOTSEL**.
5. Aparecerá una unidad de almacenamiento.
6. Copiar el archivo `.uf2`.
7. La Pico reinicia y ejecuta el firmware.

**BOOTSEL permite programar sin utilizar todavía SWD.**

---

# 19. Diagnóstico rápido

## `.pico-sdk` no existe

```powershell
Test-Path "$env:USERPROFILE\.pico-sdk"
```

Si devuelve `False`, inicializar primero un proyecto con la extensión oficial.

## Ver qué detectó el script

```powershell
.\build.ps1 practica_01_gpio -VerboseTools
```

## CMake o herramientas cambiaron

```powershell
.\build.ps1 practica_01_gpio -Clean -VerboseTools
```

## PowerShell bloquea el script

```powershell
Set-ExecutionPolicy -Scope Process Bypass
```

## Error relacionado con Picotool

Primero verificar que el script muestre:

```text
Picotool DIR : ...
```

Si no aparece, la instalación de herramientas de la extensión está incompleta.

---

# 20. Secuencia que debe seguir un alumno

```text
PRIMERA VEZ EN LA PC
--------------------
Instalar VS Code
      ↓
Instalar Raspberry Pi Pico Extension
      ↓
Crear/importar un proyecto
      ↓
SDK 2.3.1 / Board pico
      ↓
Esperar instalación de .pico-sdk


PRIMERA VEZ CON EL REPOSITORIO
------------------------------
Abrir PowerShell en la raíz
      ↓
Set-ExecutionPolicy -Scope Process Bypass
      ↓
.\build.ps1 practica_01_gpio -Clean -VerboseTools
      ↓
Comprobar herramientas
      ↓
Obtener UF2


TRABAJO NORMAL
--------------
Editar código
      ↓
Guardar
      ↓
.\build.ps1 practica_01_gpio
      ↓
UF2
      ↓
BOOTSEL
      ↓
Probar
```

---

# 21. Idea fundamental

El script simplifica el uso del entorno, pero el alumno debe reconocer qué sucede por debajo:

```text
Firmware
   ↓
CMake
   ↓
Ninja
   ↓
Cross-compiler ARM
   ↓
Linker
   ↓
ELF
   ↓
UF2
   ↓
RP2040
```

Además, durante la construcción existen herramientas que se ejecutan en la propia PC, como **Python, Picotool y PIOASM**.

Esta distinción entre **herramientas del host** y **herramientas para el target** es una de las diferencias importantes entre desarrollar una aplicación convencional y construir firmware para un microcontrolador.
