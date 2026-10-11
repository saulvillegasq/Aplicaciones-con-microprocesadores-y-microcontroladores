<#
.SYNOPSIS
    Compila una práctica del curso de Microcontroladores usando las herramientas
    instaladas por la extensión oficial "Raspberry Pi Pico" de VS Code.

.DESCRIPTION
    Este script NO instala herramientas.

    Utiliza el entorno administrado por la extensión en:

        C:\Users\<usuario>\.pico-sdk

    y localiza:

        - Raspberry Pi Pico SDK
        - CMake
        - Ninja
        - Arm GNU Toolchain
        - Python 3
        - Picotool
        - PIOASM

    Después configura CMake, compila la práctica y muestra el archivo .uf2.

.EXAMPLE
    .\build.ps1 practica_01_gpio

.EXAMPLE
    .\build.ps1 practica_01_gpio -Clean

.EXAMPLE
    .\build.ps1 practica_01_gpio -VerboseTools
#>

param(
    # Nombre de la carpeta de la práctica.
    [Parameter(Position = 0)]
    [string]$Practica = "practica_01_gpio",

    # Tarjeta objetivo. Para el curso se usa "pico".
    [string]$Board = "pico",

    # Borra build/ antes de configurar y compilar.
    [switch]$Clean,

    # Muestra las rutas exactas de las herramientas detectadas.
    [switch]$VerboseTools
)

$ErrorActionPreference = "Stop"

# =============================================================================
# 1. CONFIGURACIÓN FIJA DEL CURSO
# =============================================================================
#
# Estas versiones deben ser iguales para todos los alumnos.
# No se recomienda modificarlas salvo indicación del profesor.

$SdkVersion       = "2.3.1"
$ToolchainVersion = "15_2_Rel1"
$PicotoolVersion  = "2.3.1"

# =============================================================================
# 2. RUTAS DEL REPOSITORIO
# =============================================================================

$RepoRoot   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Join-Path $RepoRoot $Practica
$BuildDir   = Join-Path $ProjectDir "build"

if (-not (Test-Path $ProjectDir)) {
    throw @"
No existe la práctica:

    $ProjectDir

Ejemplo de uso:

    .\build.ps1 practica_01_gpio
"@
}

# =============================================================================
# 3. RUTA DEL ENTORNO INSTALADO POR LA EXTENSIÓN RASPBERRY PI PICO
# =============================================================================
#
# Se usa USERPROFILE para que el script funcione con cualquier alumno.
#
# Ejemplo:
#   C:\Users\PLATA\.pico-sdk
#   C:\Users\Alumno\.pico-sdk

$PicoRoot = Join-Path $env:USERPROFILE ".pico-sdk"

if (-not (Test-Path $PicoRoot)) {
    throw @"
No se encontró el entorno de Raspberry Pi Pico:

    $PicoRoot

Antes de utilizar este script:

1. Instale la extensión oficial "Raspberry Pi Pico" en VS Code.
2. Abra Ctrl + Shift + P.
3. Cree o importe por primera vez un proyecto Pico.
4. Seleccione Raspberry Pi Pico y SDK $SdkVersion.
5. Espere a que la extensión descargue las herramientas.
6. Verifique que exista:

    $PicoRoot
"@
}

# =============================================================================
# 4. FUNCIONES AUXILIARES
# =============================================================================

function Get-FirstFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Root,

        [Parameter(Mandatory = $true)]
        [string]$Filter
    )

    if (-not (Test-Path $Root)) {
        return $null
    }

    return Get-ChildItem `
        -Path $Root `
        -Filter $Filter `
        -File `
        -Recurse `
        -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1
}

function Add-DirectoryToPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Directory
    )

    if ($env:PATH -notlike "*$Directory*") {
        $env:PATH = "$Directory;$env:PATH"
    }
}

# =============================================================================
# 5. LOCALIZAR EL SDK Y EL TOOLCHAIN ARM
# =============================================================================

$SdkPath = Join-Path $PicoRoot "sdk\$SdkVersion"
$ToolchainPath = Join-Path $PicoRoot "toolchain\$ToolchainVersion"

if (-not (Test-Path $SdkPath)) {
    throw @"
No se encontró el Pico SDK $SdkVersion en:

    $SdkPath

Abra VS Code y permita que la extensión Raspberry Pi Pico instale esa versión.
"@
}

if (-not (Test-Path $ToolchainPath)) {
    throw @"
No se encontró el Arm GNU Toolchain $ToolchainVersion en:

    $ToolchainPath

Abra VS Code y permita que la extensión Raspberry Pi Pico complete la instalación.
"@
}

$ArmGcc = Get-FirstFile -Root $ToolchainPath -Filter "arm-none-eabi-gcc.exe"

if (-not $ArmGcc) {
    throw "No se encontró arm-none-eabi-gcc.exe dentro de $ToolchainPath"
}

# =============================================================================
# 6. LOCALIZAR CMAKE, NINJA Y PYTHON
# =============================================================================
#
# Estas herramientas también son instaladas/administradas por la extensión.
# No se presupone que estén en el PATH global de Windows.

$CMake = Get-FirstFile `
    -Root (Join-Path $PicoRoot "cmake") `
    -Filter "cmake.exe"

$Ninja = Get-FirstFile `
    -Root (Join-Path $PicoRoot "ninja") `
    -Filter "ninja.exe"

$Python = Get-FirstFile `
    -Root (Join-Path $PicoRoot "python") `
    -Filter "python.exe"

if (-not $CMake) {
    throw "No se encontró CMake dentro de $PicoRoot\cmake"
}

if (-not $Ninja) {
    throw "No se encontró Ninja dentro de $PicoRoot\ninja"
}

if (-not $Python) {
    throw @"
No se encontró Python dentro de:

    $PicoRoot\python

No es necesario instalar otro Python si la extensión puede descargarlo.
Abra VS Code y permita que la extensión Raspberry Pi Pico complete su instalación.
"@
}

# =============================================================================
# 7. LOCALIZAR HERRAMIENTAS DEL HOST: PICOTOOL Y PIOASM
# =============================================================================
#
# IMPORTANTE:
#
# arm-none-eabi-gcc compila el firmware que se ejecutará en el RP2040.
#
# Picotool y PIOASM, en cambio, son programas que se ejecutan en Windows
# durante el proceso de construcción.
#
# Si CMake no encuentra Picotool ya instalado, intentará compilarlo desde
# código fuente. Eso requiere un compilador C/C++ para Windows (por ejemplo
# MSVC), y puede generar el error:
#
#     No CMAKE_C_COMPILER could be found
#
# Por eso se indican explícitamente las herramientas precompiladas que ya
# instaló la extensión Raspberry Pi Pico.

$PicotoolRoot = Join-Path $PicoRoot "picotool\$PicotoolVersion"
$PicotoolConfig = Get-FirstFile `
    -Root $PicotoolRoot `
    -Filter "picotoolConfig.cmake"

if (-not $PicotoolConfig) {
    # Algunos paquetes pueden usar el nombre con guion.
    $PicotoolConfig = Get-FirstFile `
        -Root $PicotoolRoot `
        -Filter "picotool-config.cmake"
}

if (-not $PicotoolConfig) {
    throw @"
No se encontró la configuración de Picotool $PicotoolVersion en:

    $PicotoolRoot

Abra VS Code y permita que la extensión Raspberry Pi Pico descargue Picotool.
"@
}

$PicotoolDir = Split-Path -Parent $PicotoolConfig.FullName

$ToolsRoot = Join-Path $PicoRoot "tools\$SdkVersion"
$PioasmConfig = Get-FirstFile `
    -Root $ToolsRoot `
    -Filter "pioasmConfig.cmake"

if (-not $PioasmConfig) {
    $PioasmConfig = Get-FirstFile `
        -Root $ToolsRoot `
        -Filter "pioasm-config.cmake"
}

if (-not $PioasmConfig) {
    throw @"
No se encontró PIOASM para el SDK $SdkVersion en:

    $ToolsRoot

Abra VS Code y permita que la extensión Raspberry Pi Pico complete la instalación.
"@
}

$PioasmDir = Split-Path -Parent $PioasmConfig.FullName

# =============================================================================
# 8. PICO-VSCODE.CMAKE
# =============================================================================
#
# La extensión oficial instala este archivo. Los CMakeLists.txt del curso
# también lo incluyen para reproducir la configuración de un proyecto creado
# directamente por la extensión.

$PicoVscodeCMake = Join-Path $PicoRoot "cmake\pico-vscode.cmake"

if (-not (Test-Path $PicoVscodeCMake)) {
    throw @"
No se encontró:

    $PicoVscodeCMake

La instalación de la extensión Raspberry Pi Pico está incompleta.
"@
}

# =============================================================================
# 9. PREPARAR EL ENTORNO ÚNICAMENTE PARA ESTA EJECUCIÓN
# =============================================================================
#
# No se modifica permanentemente el PATH de Windows.

$env:PICO_SDK_PATH = $SdkPath
$env:PICO_TOOLCHAIN_PATH = $ToolchainPath

Add-DirectoryToPath -Directory (Split-Path -Parent $CMake.FullName)
Add-DirectoryToPath -Directory (Split-Path -Parent $Ninja.FullName)
Add-DirectoryToPath -Directory (Split-Path -Parent $ArmGcc.FullName)
Add-DirectoryToPath -Directory (Split-Path -Parent $Python.FullName)

# =============================================================================
# 10. COPIAR EL ARCHIVO OFICIAL pico_sdk_import.cmake
# =============================================================================

$SdkImportSource = Join-Path $SdkPath "external\pico_sdk_import.cmake"
$SdkImportTarget = Join-Path $RepoRoot "pico_sdk_import.cmake"

if (-not (Test-Path $SdkImportSource)) {
    throw "No se encontró $SdkImportSource"
}

Copy-Item $SdkImportSource $SdkImportTarget -Force

# =============================================================================
# 11. MOSTRAR DIAGNÓSTICO
# =============================================================================

Write-Host ""
Write-Host "=== Entorno Raspberry Pi Pico ==="
Write-Host "Práctica    : $Practica"
Write-Host "Board       : $Board"
Write-Host "SDK         : $SdkVersion"
Write-Host "Toolchain   : $ToolchainVersion"
Write-Host "Picotool    : $PicotoolVersion"

if ($VerboseTools) {
    Write-Host ""
    Write-Host "Herramientas detectadas:"
    Write-Host "  Pico SDK       : $SdkPath"
    Write-Host "  Toolchain ARM  : $ToolchainPath"
    Write-Host "  GCC ARM        : $($ArmGcc.FullName)"
    Write-Host "  CMake          : $($CMake.FullName)"
    Write-Host "  Ninja          : $($Ninja.FullName)"
    Write-Host "  Python         : $($Python.FullName)"
    Write-Host "  Picotool DIR   : $PicotoolDir"
    Write-Host "  PIOASM DIR     : $PioasmDir"
    Write-Host "  pico-vscode    : $PicoVscodeCMake"
}

# =============================================================================
# 12. LIMPIAR LA CONSTRUCCIÓN ANTERIOR
# =============================================================================

if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host ""
    Write-Host "Eliminando build anterior..."
    Remove-Item -Recurse -Force $BuildDir
}

# =============================================================================
# 13. CONFIGURAR EL PROYECTO CON CMAKE
# =============================================================================
#
# -S : carpeta con el CMakeLists.txt
# -B : carpeta de construcción
# -G : generador utilizado (Ninja)
#
# También se pasan explícitamente las rutas del entorno de la extensión.

Write-Host ""
Write-Host "Configurando proyecto..."

& $CMake.FullName `
    -S $ProjectDir `
    -B $BuildDir `
    -G Ninja `
    "-DPICO_BOARD=$Board" `
    "-DPICO_SDK_PATH=$SdkPath" `
    "-DPICO_TOOLCHAIN_PATH=$ToolchainPath" `
    "-DPython3_EXECUTABLE=$($Python.FullName)" `
    "-Dpicotool_DIR=$PicotoolDir" `
    "-Dpioasm_DIR=$PioasmDir"

if ($LASTEXITCODE -ne 0) {
    throw @"
CMake terminó con error durante la configuración.

Pruebe primero:

    .\build.ps1 $Practica -Clean -VerboseTools
"@
}

# =============================================================================
# 14. COMPILAR
# =============================================================================

Write-Host ""
Write-Host "Compilando..."

& $CMake.FullName --build $BuildDir

if ($LASTEXITCODE -ne 0) {
    throw "La compilación terminó con error."
}

# =============================================================================
# 15. BUSCAR EL UF2 GENERADO
# =============================================================================

$Uf2 = Get-ChildItem `
    -Path $BuildDir `
    -Filter "*.uf2" `
    -File `
    -Recurse `
    -ErrorAction SilentlyContinue |
    Where-Object {
        $_.FullName -notlike "*\_deps\*"
    } |
    Select-Object -First 1

Write-Host ""
Write-Host "=== Compilación terminada ==="

if ($Uf2) {
    Write-Host "UF2 generado:"
    Write-Host "  $($Uf2.FullName)"
}
else {
    Write-Warning "La compilación terminó, pero no se encontró el archivo .uf2 de la práctica."
}

Write-Host ""
