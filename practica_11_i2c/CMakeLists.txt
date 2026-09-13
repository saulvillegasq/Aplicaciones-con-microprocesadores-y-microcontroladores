cmake_minimum_required(VERSION 3.13)

# Versiones fijadas para el curso
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Integración con la extensión oficial Raspberry Pi Pico de VS Code
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(picoVscode "${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake")

if(EXISTS "${picoVscode}")
    include("${picoVscode}")
else()
    message(FATAL_ERROR
        "No se encontro pico-vscode.cmake. "
        "Inicialice primero la extension Raspberry Pi Pico en VS Code.")
endif()

# Debe importarse antes de project()
include(../pico_sdk_import.cmake)

project(practica_11_i2c C CXX ASM)

pico_sdk_init()

add_executable(practica_11_i2c
    src/main.c
    drivers/i2c_bus.c
)

target_include_directories(practica_11_i2c PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
)
target_link_libraries(practica_11_i2c
    pico_stdlib
    hardware_i2c
)

pico_enable_stdio_usb(practica_11_i2c 1)
pico_enable_stdio_uart(practica_11_i2c 0)

pico_add_extra_outputs(practica_11_i2c)
