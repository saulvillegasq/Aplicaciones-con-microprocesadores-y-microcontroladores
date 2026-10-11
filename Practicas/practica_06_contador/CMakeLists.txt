
cmake_minimum_required(VERSION 3.13)

# ============================================================
# 1. VERSIONES DEL ENTORNO
# ============================================================

set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ============================================================
# 2. CONFIGURACIÓN DEL PICO SDK
# ============================================================

# Obtener el directorio de usuario.
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# Modificar esta ruta según la ubicación del SDK.
# Ejemplo: D:/.pico-sdk/cmake/pico-vscode.cmake
set(picoVscode "${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake")

if(EXISTS "${picoVscode}")
    include("${picoVscode}")
else()
    message(FATAL_ERROR
        "No se encontro pico-vscode.cmake. "
        "Verifique la ruta del Pico SDK.")
endif()


# ============================================================
# 3. IMPORTACIÓN DEL SDK
# ============================================================

# Debe importarse antes de project().
include(../pico_sdk_import.cmake)

project(practica_06_contador C CXX ASM)

pico_sdk_init()


# ============================================================
# 4. ARCHIVOS FUENTE
# ============================================================

add_executable(practica_06_contador
    src/main.c
)


# ============================================================
# 5. BIBLIOTECAS
# ============================================================

target_link_libraries(practica_06_contador
    pico_stdlib       # Funciones básicas del SDK
    hardware_gpio     # Configuración y control de GPIO
    hardware_pwm      # Conteo de pulsos mediante PWM
    hardware_timer    # Temporizador y alarmas de hardware
    hardware_sync     # Protección de secciones críticas
)


# ============================================================
# 6. COMUNICACIÓN SERIAL
# ============================================================

# Habilitar printf() mediante USB CDC.
pico_enable_stdio_usb(practica_06_contador 1)

# Deshabilitar salida UART física.
pico_enable_stdio_uart(practica_06_contador 0)


# ============================================================
# 7. ARCHIVOS DE SALIDA
# ============================================================

# Generar ELF, BIN, HEX y UF2.
pico_add_extra_outputs(practica_06_contador)
