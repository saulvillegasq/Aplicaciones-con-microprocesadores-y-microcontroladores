# Aplicaciones con microprocesadores y microcontroladores

Repositorio de materiales y prácticas del curso, desarrollado principalmente en **lenguaje C** para las tarjetas **Raspberry Pi Pico (RP2040)**, **Raspberry Pi Pico W (RP2040)** y **Raspberry Pi Pico 2 (RP2350)**.

El curso aborda el firmware como la integración de **software, hardware y tiempo**: no basta con que un programa compile; debe interactuar correctamente con los periféricos y respetar sus condiciones eléctricas y temporales.

## 1. Contenido del repositorio

```text
microcontroladores_raspberry_pico/
├── Archivos fuente/
│   ├── build.ps1
│   ├── CMakeLists.txt
│   ├── P_CMakeLists.txt
│   └── pico_sdk_import.cmake
├── Contenido de clase/
│   ├── BloqueA.pdf
│   └── BloqueB.pdf
├── Documentos/
│   ├── Guia_del_repositorio.md
│   └── Compendio_funciones_Raspberry_Pi_Pico_SDK.*
└── Practicas/
    └── ...
```

- **Contenido de clase:** presentaciones y material expuesto durante las sesiones de clase.
- **Documentos:** instrucciones de configuración y compilación; también incluye el **`Compendio_funciones_Raspberry_Pi_Pico_SDK`**, una tabla de consulta sobre funciones del SDK. La extensión del archivo dependerá del formato publicado.
- **Archivos fuente:** utilidades y plantillas para preparar la construcción de proyectos.
- **Practicas:** carpetas de los proyectos de laboratorio, que se incorporan conforme avanza el curso.

### Archivos de apoyo para la construcción

| Archivo | Propósito |
|---|---|
| `build.ps1` | Localiza el entorno del SDK y las herramientas, configura y construye una práctica. |
| `CMakeLists.txt` | Archivo CMake del nivel raíz; **no** construye todas las prácticas por sí solo. |
| `P_CMakeLists.txt` | Plantilla del `CMakeLists.txt` individual de una práctica. |
| `pico_sdk_import.cmake` | Importa el Pico SDK durante la configuración con CMake. |

## 2. Contenido de las prácticas

Las prácticas avanzan progresivamente por entradas y salidas digitales, interrupciones, temporizadores y PWM, conversión analógica-digital y comunicaciones.

Una práctica puede organizarse de la siguiente manera:

```text
practica_xx_nombre/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.c
│   └── application.c
├── include/
│   └── application.h
├── drivers/
└── build/           # carpeta generada al compilar; no se versiona
```

No todas las prácticas necesitan desde el inicio todos estos archivos: la separación entre aplicación y drivers se introduce progresivamente.

**Criterio arquitectónico:**

```text
Aplicación → Drivers propios → Pico SDK → Registros → Periféricos → Hardware físico
```

## 3. Descargar el entorno del curso

El SDK y las herramientas de construcción se distribuyen en un paquete común:

**[Descargar el SDK del curso (Google Drive)](https://drive.google.com/file/d/1MnlJRK9qLSiv0gsdrPByhGWsEHjmQC25/view?usp=sharing)**

Después de descargarlo, extraerlo completamente en una ruta corta, por ejemplo:

```text
D:\.pico-sdk
```

La carpeta indicada debe contener subdirectorios como `sdk/`, `toolchain/`, `cmake/`, `ninja/` y otras herramientas. Se utiliza el **Pico SDK 2.3.1** y el **toolchain ARM `15_2_Rel1`** previstos para el curso.

## 4. Preparar los archivos para compilar

**Importante:** las carpetas `Archivos fuente/` y `Practicas/` organizan los materiales. El `build.ps1` entregado busca la práctica **junto al propio script**. Por ello, para usarlo sin cambiar su lógica, crear una carpeta de trabajo como esta:

```text
carpeta_de_trabajo/
├── build.ps1
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── practica_00_blink/
    ├── CMakeLists.txt
    └── src/
        └── main.c
```

Copiar allí los archivos necesarios desde `Archivos fuente/` y la carpeta completa de la práctica desde `Practicas/`. Cada práctica debe conservar su propio `CMakeLists.txt`; `P_CMakeLists.txt` es solamente una plantilla.

Ajustar **dos rutas** según la ubicación elegida:

**En `build.ps1`**, localizar `$PicoRoot` y establecer la raíz del paquete:

```powershell
$PicoRoot = 'D:\.pico-sdk'
```

**En el `CMakeLists.txt` de cada práctica**, localizar `picoVscode` y apuntarlo al archivo correspondiente:

```cmake
set(picoVscode "D:/.pico-sdk/cmake/pico-vscode.cmake")
```

En CMake se recomienda usar `/` como separador de rutas. Ambas configuraciones deben referirse al **mismo entorno**.

## 5. Compilación: Pico, Pico W y Pico 2

Abrir PowerShell **en la carpeta donde se encuentra `build.ps1`**. Para una construcción limpia de la práctica inicial:

| Tarjeta | MCU | Comando |
|---|---|---|
| Raspberry Pi Pico | RP2040 | ` .\build.ps1 practica_00_blink -Board pico -Clean -VerboseTools` |
| Raspberry Pi Pico W | RP2040 | ` .\build.ps1 practica_00_blink -Board pico_w -Clean -VerboseTools` |
| Raspberry Pi Pico 2 | RP2350 | ` .\build.ps1 practica_00_blink -Board pico2 -Clean -VerboseTools` |

También existe el identificador `pico2_w` para Raspberry Pi Pico 2 W cuando corresponda.

- `-Board` selecciona la placa objetivo; el valor predeterminado del script es `pico`.
- `-Clean` elimina la carpeta de construcción de la práctica antes de configurar y compilar otra vez; usarlo al **cambiar de tarjeta o de configuración**.
- `-VerboseTools` muestra las rutas de las herramientas detectadas.

Para compilar nuevamente sin limpiar, por ejemplo:

```powershell
.\build.ps1 practica_00_blink -Board pico
```

**Nota:** el programa también debe ser compatible con el hardware elegido. Por ejemplo, el LED integrado se controla de forma distinta en algunas tarjetas con conectividad inalámbrica.

## 6. Nombres personalizables

El archivo **`build.ps1` puede renombrarse** (por ejemplo, `compilar.ps1`). En ese caso, cambiar su nombre también en el comando:

```powershell
.\compilar.ps1 practica_00_blink -Board pico2 -Clean
```

La carpeta **`build/` también puede tener otro nombre**, como `outputs/` o `compilacion/`. Para ello modificar dentro del script:

```powershell
$BuildDir = Join-Path $ProjectDir "outputs"
```

No basta con renombrar manualmente la carpeta: el script debe apuntar al nuevo nombre. Si queda una carpeta de construcción anterior, puede eliminarse cuando ya no se necesite.

## 7. Firmware generado y programación

Al terminar correctamente la construcción, se obtiene un archivo **`.uf2`** en la carpeta de salida de la práctica (`build/` u otro nombre configurado). Para programar mediante BOOTSEL:

1. Desconectar la tarjeta del USB.
2. Mantener presionado **BOOTSEL** mientras se conecta a la computadora.
3. Soltar BOOTSEL cuando aparezca la unidad de almacenamiento.
4. Copiar el archivo `.uf2` generado a esa unidad.
5. Esperar a que la tarjeta se reinicie y ejecute el firmware.

**Compilar no equivale a programar:** `build.ps1` construye el firmware; la carga mediante BOOTSEL es una operación posterior.

## 8. Documentación recomendada

Para instrucciones detalladas, consultar **`Documentos/Guia_del_repositorio.md`**. Para consultar APIs y funciones durante el desarrollo de prácticas, utilizar **`Documentos/Compendio_funciones_Raspberry_Pi_Pico_SDK`**.

También pueden consultarse las fuentes oficiales:

- [Documentación del Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)
- [Referencia de la API del SDK](https://www.raspberrypi.com/documentation/pico-sdk/)
- [Ejemplos oficiales de Raspberry Pi Pico](https://github.com/raspberrypi/pico-examples)

**Recomendación:** antes de pedir ayuda con un error de compilación, ejecutar el comando con `-Clean -VerboseTools` y conservar el mensaje de error completo.
