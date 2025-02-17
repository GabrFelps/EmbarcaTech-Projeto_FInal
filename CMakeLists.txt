# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Projeto_Embarca C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(Projeto_Embarca Projeto_Embarca.c )

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/pico-ssd1306-main)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/bmp280_driver-main)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation)

pico_set_program_name(Projeto_Embarca "Projeto_Embarca")
pico_set_program_version(Projeto_Embarca "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Projeto_Embarca 0)
pico_enable_stdio_usb(Projeto_Embarca 1)

# Add the standard library to the build
target_link_libraries(Projeto_Embarca
        pico_stdlib
        hardware_adc
        hardware_pwm
        pico-ssd1306
        bmp280_driver
        ws2812b_animation
        )

# Add the standard include files to the build
target_include_directories(Projeto_Embarca PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/libs/pico-ssd1306-main
        ${CMAKE_CURRENT_LIST_DIR}/libs/bmp280_driver
)

pico_add_extra_outputs(Projeto_Embarca)

set(TARGET_NAME "ws2812b_animation")

if (NOT TARGET ${TARGET_NAME})
    add_library(${TARGET_NAME} INTERFACE)

    target_sources(${TARGET_NAME} INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/ws2812b_animation.c
            ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/inc/utf8-iterator/source/utf-8.c
    )

    target_include_directories(${TARGET_NAME} INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/inc/
            ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/inc/CP0-EU/
            ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/inc/utf8-iterator/source/
    )

    pico_generate_pio_header(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/libs/RP2040-WS2812B-Animation/ws2812.pio)

    target_link_libraries(${TARGET_NAME} INTERFACE
        pico_stdlib
        hardware_pio
    )
endif()
