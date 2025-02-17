cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



option(BMP280_DRIVER_PICO_SDK_PATH CACHE pico-sdk)
option(BMP280_DRIVER_BUILD_EXAMPLE CACHE OFF)
option(BMP280_DRIVER_FORCE_C_COMPILER CACHE OFF)



if (DEFINED ENV{PICO_SDK_PATH})
message("BMP280 DRIVER: Using environment variable PICO_SDK_PATH")
set(BMP280_DRIVER_PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

if(DEFINED PICO_SDK_PATH)
message("BMP280 DRIVER: Using command defined variable PICO_SDK_PATH")
set(BMP280_DRIVER_PICO_SDK_PATH ${PICO_SDK_PATH})
endif()


message("BMP280 DRIVER: PICO_SDK_PATH is " ${BMP280_DRIVER_PICO_SDK_PATH})
include(${BMP280_DRIVER_PICO_SDK_PATH}/external/pico_sdk_import.cmake)



if (BMP280_DRIVER_FORCE_C_COMPILER)
project(bmp280_driver C)
else()
project(bmp280_driver C CXX ASM)
endif(BMP280_DRIVER_FORCE_C_COMPILER)



pico_sdk_init()



function(bmp280_build_driver)
add_library(bmp280_driver STATIC bmp280_driver/src/bmp280_driver.c)
target_include_directories(bmp280_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bmp280_driver/include
)
target_link_libraries(bmp280_driver PUBLIC
    pico_stdlib
    hardware_i2c
)
set_target_properties(bmp280_driver PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()



function(bmp280_build_example)
message("BMP280 DRIVER: building example code")
add_executable(bmp280_example
    example/src/example.c
)
target_include_directories(bmp280_example PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/example/include
)
target_link_libraries(bmp280_example PUBLIC 
    bmp280_driver
)
pico_enable_stdio_uart(bmp280_example 0)
pico_enable_stdio_usb(bmp280_example 1)
pico_add_extra_outputs(bmp280_example)
endfunction()



function(main)
bmp280_build_driver()
if(BMP280_DRIVER_BUILD_EXAMPLE) 
bmp280_build_example()
endif(BMP280_DRIVER_BUILD_EXAMPLE)
endfunction()



main()