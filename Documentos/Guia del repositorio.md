# Guía del repositorio y primera compilación

**Asignatura:** Aplicaciones con microprocesadores y microcontroladores  
**Plataformas:** Raspberry Pi Pico, Pico W y Pico 2 (familias RP2040 y RP2350)  
**Lenguaje:** C  
**Entorno de referencia:** Windows, PowerShell, Pico SDK 2.3.1

## 1. ¿Qué contiene el repositorio?

Este repositorio reúne los materiales de clase, las guías de consulta, los archivos auxiliares de construcción y las prácticas de laboratorio.

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
│   └── Compendio_funciones_Raspberry_Pi_Pico_SDK
└── Practicas/
    └── ... (prácticas disponibles durante el curso)
```

- **Contenido de clase:** contiene las presentaciones y el material que el profesor expone durante las sesiones. Los PDF están organizados por bloques temáticos.
- **Documentos:** contiene guías para la instalación, la arquitectura del firmware, la compilación y la programación. **Incluye el documento `Compendio_funciones_Raspberry_Pi_Pico_SDK`**, que contiene una tabla de consulta de las funciones del Pico SDK. Sirve para localizar funciones, reconocer su finalidad y orientar su utilización durante las prácticas. Es una guía de apoyo y no sustituye la documentación oficial de las API.
- **Archivos fuente:** contiene los archivos auxiliares utilizados para organizar y configurar la construcción de los proyectos.
- **Practicas:** contiene los proyectos de laboratorio. Las prácticas se incorporarán y desarrollarán conforme avance el curso.

> **Importante:** la distribución del material que aparece en VS Code no coincide necesariamente con la estructura de trabajo requerida por el `build.ps1` original. El script busca la carpeta de la práctica junto al propio script. La sección 4 explica cómo preparar una copia operativa sin alterar los materiales de referencia.

## 2. Archivos auxiliares de construcción

| Archivo | ¿Qué hace? | ¿Qué debe modificar el alumno? |
|---|---|---|
| `build.ps1` | Localiza el SDK y las herramientas, prepara CMake, ejecuta la construcción y muestra dónde quedó el firmware `.uf2`. | Ajustar `$PicoRoot` a la ubicación del SDK descargado. |
| `CMakeLists.txt` de la raíz | Identifica el repositorio; no construye directamente las prácticas. | Normalmente no necesita modificación. |
| `P_CMakeLists.txt` | Plantilla del archivo `CMakeLists.txt` de una práctica; el ejemplo corresponde a `practica_00_blink`. | Al emplearla en una práctica, guardarla como `CMakeLists.txt` y revisar el nombre del proyecto, los archivos fuente y `picoVscode`. |
| `pico_sdk_import.cmake` | Permite importar el Pico SDK al sistema de construcción de CMake. | Normalmente no se modifica. |

**El nombre `build.ps1` no es obligatorio.** Se puede cambiar por `compilar.ps1`, `construir.ps1` u otro nombre terminado en `.ps1`. En ese caso, todos los comandos deben utilizar el nombre nuevo. Por ejemplo:

```powershell
.\compilar.ps1 practica_00_blink -Board pico -Clean -VerboseTools
```

Renombrar el archivo no cambia automáticamente su funcionamiento interno. El script no es un compilador: coordina CMake, Ninja, el compilador cruzado ARM y otras herramientas.

## 3. Descargar y preparar el SDK del curso

El paquete completo empleado durante el curso se encuentra en el enlace facilitado por el profesor:

**[Descargar SDK del curso desde Google Drive](https://drive.google.com/file/d/1MnlJRK9qLSiv0gsdrPByhGWsEHjmQC25/view?usp=sharing)**

1. Descargar el paquete y extraerlo completamente si está comprimido.
2. Colocar la carpeta principal del entorno en una ruta corta, como `D:\.pico-sdk` o `C:\pico-sdk`.
3. Evitar rutas demasiado largas o con numerosos niveles de carpetas, pues pueden dificultar la configuración y el diagnóstico de errores.
4. Confirmar que la carpeta elegida contiene las subcarpetas esperadas por el script.

Ejemplo de organización interna del paquete:

```text
D:\.pico-sdk\
├── sdk\2.3.1\
├── toolchain\15_2_Rel1\
├── cmake\pico-vscode.cmake
├── ninja\
├── python\
├── picotool\2.3.1\
└── tools\2.3.1\
```

Desde PowerShell pueden comprobarse algunas rutas:

```powershell
Test-Path 'D:\.pico-sdk\sdk\2.3.1'
Test-Path 'D:\.pico-sdk\toolchain\15_2_Rel1'
Test-Path 'D:\.pico-sdk\cmake\pico-vscode.cmake'
```

Los tres comandos deben devolver `True` si se eligió esa ruta y el contenido corresponde a la estructura esperada. El paquete también debe proporcionar las herramientas auxiliares que requiere el script, como CMake, Ninja, Python, Picotool y PIOASM.

## 4. Preparar la carpeta de trabajo

El script original obtiene las rutas con instrucciones equivalentes a:

```powershell
$RepoRoot   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Join-Path $RepoRoot $Practica
```

Por esa razón, para **utilizarlo sin cambiar su lógica**, la práctica y el script deben estar al mismo nivel; el `CMakeLists.txt` de la práctica importa a su vez `../pico_sdk_import.cmake`.

Prepare una **copia de trabajo** con esta disposición:

```text
mi_carpeta_de_trabajo/
├── build.ps1
├── CMakeLists.txt              (configuración de la raíz)
├── pico_sdk_import.cmake
├── practica_00_blink/
│   ├── CMakeLists.txt          (configuración propia de la práctica)
│   └── src/
│       └── main.c
└── practica_01_gpio/
    ├── CMakeLists.txt
    └── src/
        └── main.c
```

Copie los archivos auxiliares desde `Archivos fuente/` y la carpeta de la práctica desde `Practicas/`. No es necesario modificar los originales del repositorio. Si se decide conservar físicamente las prácticas dentro de `Practicas/` y el script dentro de `Archivos fuente/`, será necesario adaptar las rutas relativas del script y de CMake; modificar solamente la ruta del SDK no es suficiente.

## 5. Modificar la ubicación del SDK en `build.ps1`

Abra `build.ps1` (o el nombre que le haya dado). Busque la asignación de `$PicoRoot`:

```powershell
$PicoRoot = Join-Path $env:USERPROFILE '.pico-sdk'
```

Reemplácela por la ruta real donde extrajo el paquete, por ejemplo:

```powershell
$PicoRoot = 'D:\.pico-sdk'
```

La variable apunta a la **carpeta principal del paquete**, no a `sdk\2.3.1`. El script calcula las subrutas internamente.

## 6. Modificar el archivo `CMakeLists.txt` de cada práctica

Abra, por ejemplo, `practica_00_blink/CMakeLists.txt`. Localice la instrucción:

```cmake
set(picoVscode "${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake")
```

Sustitúyala por la ruta correspondiente a su equipo:

```cmake
set(picoVscode "D:/.pico-sdk/cmake/pico-vscode.cmake")
```

Si el paquete está en `C:\pico-sdk`, utilice:

```cmake
set(picoVscode "C:/pico-sdk/cmake/pico-vscode.cmake")
```

Se recomienda emplear `/` en las rutas de CMake. **La ruta de `picoVscode` y la variable `$PicoRoot` deben apuntar al mismo paquete**, aunque la primera llega a un archivo específico y la segunda identifica la carpeta principal.

No confunda el `CMakeLists.txt` de la raíz con el `CMakeLists.txt` de cada práctica: cumplen funciones distintas.

## 7. Seleccionar la tarjeta y compilar

El script compartido admite un parámetro denominado **`-Board`**, el cual permite indicar la tarjeta objetivo. Los identificadores del SDK para las placas contempladas son:

| Tarjeta | Microcontrolador | Valor de `-Board` |
|---|---|---|
| Raspberry Pi Pico | RP2040 | `pico` |
| Raspberry Pi Pico W | RP2040 | `pico_w` |
| Raspberry Pi Pico 2 | RP2350 | `pico2` |
| Raspberry Pi Pico 2 W | RP2350 | `pico2_w` |

La Pico 2 W se incluye como referencia adicional. El parámetro `-Board` selecciona la configuración de compilación, pero no garantiza por sí solo que una práctica esté adaptada a todas las placas. En el curso deben utilizarse los modelos indicados por el profesor.

Abra PowerShell en la carpeta que contiene `build.ps1`. Si Windows impide ejecutar scripts, habilite su ejecución **solo para la sesión actual**:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
```

**Primera compilación de `practica_00_blink` según la tarjeta:**

```powershell
# Raspberry Pi Pico
.\build.ps1 practica_00_blink -Board pico -Clean -VerboseTools

# Raspberry Pi Pico W
.\build.ps1 practica_00_blink -Board pico_w -Clean -VerboseTools

# Raspberry Pi Pico 2
.\build.ps1 practica_00_blink -Board pico2 -Clean -VerboseTools

# Raspberry Pi Pico 2 W (si se utiliza)
.\build.ps1 practica_00_blink -Board pico2_w -Clean -VerboseTools
```

Si no se proporciona `-Board`, el script usa **`pico`** por defecto. Esto no significa que las otras tarjetas sean intercambiables: la compilación debe corresponder al hardware real.

**Cuando se cambia de modelo de tarjeta, se debe reconstruir limpiamente con `-Clean`.** CMake conserva configuraciones de compilaciones anteriores en la carpeta de construcción (`build/` por defecto); reutilizarlas con otra placa puede provocar errores o resultados incorrectos.

Para compilaciones posteriores de la misma práctica y placa, si solo se modifica el código fuente, normalmente basta con:

```powershell
.\build.ps1 practica_00_blink -Board pico2
```

Las opciones pueden combinarse:

| Opción | Función |
|---|---|
| `-Board pico` | Compila para la Raspberry Pi Pico original. |
| `-Board pico_w` | Compila para la Pico W. |
| `-Board pico2` | Compila para la Pico 2. |
| `-Clean` | Elimina la carpeta de construcción configurada (`build/` por defecto) y realiza una construcción limpia. |
| `-VerboseTools` | Muestra las rutas y herramientas detectadas. |

> **Compatibilidad de código:** cambiar `-Board` prepara las opciones de construcción para la placa seleccionada, pero no adapta automáticamente todos los programas. La Pico W, por ejemplo, utiliza una conexión diferente para su LED integrado; puede necesitarse código específico para controlar ese LED a través del chip inalámbrico. La Pico 2 utiliza un RP2350, por lo que las aplicaciones que acceden directamente a registros o características específicas del RP2040 deben revisarse.

## 8. Carpeta de construcción y archivos de salida

Por defecto, **la carpeta que almacena los archivos generados se llama `build`**, pero este nombre **no es obligatorio**. Puede cambiarse por `outputs`, `compilacion` u otro nombre, siempre que la modificación se realice en el script `build.ps1` (o en el nombre que haya recibido ese script).

En el archivo, localice la definición de `$BuildDir`, dentro de la sección de rutas del repositorio:

```powershell
$BuildDir = Join-Path $ProjectDir "build"
```

Para utilizar `outputs`, sustitúyala por:

```powershell
$BuildDir = Join-Path $ProjectDir "outputs"
```

A partir de ese cambio, el script utiliza la nueva carpeta para **configurar CMake, compilar, buscar el `.uf2` y limpiar los archivos generados cuando se indica `-Clean`**. No es necesario modificar el comando de compilación:

```powershell
.\build.ps1 practica_00_blink -Board pico2 -Clean -VerboseTools
```

La estructura generada será, por ejemplo:

```text
practica_00_blink/
├── CMakeLists.txt
├── src/
└── outputs/
    ├── CMakeCache.txt
    ├── ... .elf
    ├── ... .bin
    ├── ... .hex
    └── ... .uf2
```

**Importante:** no basta con cambiar manualmente el nombre de una carpeta `build/` que ya existe; debe modificarse `$BuildDir`. Tras cambiar el nombre, es recomendable realizar una construcción limpia. Si quedó una carpeta `build/` anterior, puede eliminarse manualmente una vez comprobado que no contiene archivos necesarios: el parámetro `-Clean` solo limpia la carpeta indicada por el valor **actual** de `$BuildDir`.

La carpeta de construcción contiene archivos **generados**, no código fuente; no debe utilizarse para almacenar el programa de la práctica. Si se utiliza Git, conviene excluir `build/`, `outputs/` o el nombre que se haya elegido mediante `.gitignore`.

## 9. ¿Dónde queda el firmware y cómo se programa?

Al compilar se crea la carpeta de construcción configurada (`build/` por defecto, o `outputs/` si se cambió como se explicó):

```text
practica_00_blink/
└── build/
    ├── ... .elf
    ├── ... .bin
    ├── ... .hex
    └── ... .uf2
```

El script busca y presenta la ruta del archivo `.uf2` generado. Para cargarlo mediante **BOOTSEL**:

1. Desconecte la tarjeta de la computadora.
2. Mantenga presionado el botón **BOOTSEL**.
3. Conecte el cable USB y suelte BOOTSEL.
4. La tarjeta aparecerá como una unidad de almacenamiento.
5. Copie el archivo `.uf2` generado para **esa tarjeta** a la unidad.
6. La placa se reiniciará y ejecutará el firmware.

**Compilar no es programar:** `build.ps1` construye el archivo, pero no lo copia automáticamente a la placa.

## 10. Problemas comunes

| Problema | Qué verificar |
|---|---|
| `No existe la práctica` | Que la carpeta de la práctica esté junto al script en la copia operativa. |
| No se encuentra el SDK | Que `$PicoRoot` apunte a la carpeta principal correcta. |
| No se encuentra `pico-vscode.cmake` | Que la ruta `picoVscode` del proyecto exista y apunte al mismo paquete. |
| Errores de configuración al cambiar de placa | Ejecutar con `-Board` correcto y `-Clean`. |
| Errores por rutas anteriores de CMake | Reconstruir con `-Clean -VerboseTools`. |
| Falta Picotool o PIOASM | Verificar que el paquete esté completo y mantenga su estructura. |
| No se obtiene `.uf2` | Revisar los errores de compilación y que el proyecto invoque `pico_add_extra_outputs(...)`. |
| La placa compila pero el LED no funciona | Verificar el modelo y cómo está conectado su LED; no asumir que el firmware del LED es idéntico para todos. |
| PowerShell impide ejecutar el script | Revisar la política de ejecución de esa sesión. |

## 11. Lista de comprobación antes del laboratorio

- [ ] Descargué y extraje el SDK del enlace compartido.
- [ ] Confirmé la estructura interna del paquete y sus versiones.
- [ ] Preparé la copia de trabajo con el script junto a la carpeta de la práctica.
- [ ] Modifiqué `$PicoRoot` en el script.
- [ ] Modifiqué `picoVscode` en el `CMakeLists.txt` de la práctica.
- [ ] Identifiqué si mi placa es Pico, Pico W o Pico 2.
- [ ] Compilé con el identificador `-Board` correspondiente y con `-Clean` en la primera configuración.
- [ ] Identifiqué el nombre de la carpeta de construcción (`build/` u otro configurado en `$BuildDir`).
- [ ] Encontré el archivo `.uf2` generado.
- [ ] Programé la placa utilizando BOOTSEL.
- [ ] Consulté la **`Compendio_funciones_Raspberry_Pi_Pico_SDK`** que se encuentra en `Documentos/` cuando necesité identificar funciones de la API.

## 12. Referencias

- [Documentación oficial del Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).
- [Repositorio oficial de Pico SDK](https://github.com/raspberrypi/pico-sdk).
- [Referencia de funciones del SDK](https://www.raspberrypi.com/documentation/pico-sdk/).

El documento `Documentos/Compendio_funciones_Raspberry_Pi_Pico_SDK` funciona como guía práctica de consulta del curso. Para parámetros, tipos de datos, restricciones y detalles específicos de cada versión, consulte también la referencia oficial.
