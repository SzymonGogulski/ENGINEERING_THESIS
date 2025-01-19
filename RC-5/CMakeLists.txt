cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-tflmicro-project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

# Add the main executable
add_executable(main "main.cpp")

target_sources(main
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/RC5.cpp
  )


# Link the library to the executable
target_link_libraries(main 
hardware_pwm
hardware_timer
pico_stdlib
hardware_adc 
hardware_dma 
hardware_irq)

# Enable USB output and other standard features
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# Add the necessary build output for the target
pico_add_extra_outputs(main)