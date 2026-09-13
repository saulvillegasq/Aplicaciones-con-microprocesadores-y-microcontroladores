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

project(practica_07_counter_capture C CXX ASM)

pico_sdk_init()

add_executable(practica_07_counter_capture
    src/main.c
)

target_link_libraries(practica_07_counter_capture
    pico_stdlib
    hardware_gpio
    hardware_timer
)

pico_enable_stdio_usb(practica_07_counter_capture 1)
pico_enable_stdio_uart(practica_07_counter_capture 0)

pico_add_extra_outputs(practica_07_counter_capture)
