cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-tflmicro-project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

# Add cmsis core
set(CMSISCORE ${CMAKE_CURRENT_LIST_DIR}/src/CMSIS_5/CMSIS/Core)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/CMSIS-DSP/Source bin_dsp)

# Add the main executable
add_executable(main "main.cpp")

# target_sources(main
#   PRIVATE
#   ${CMAKE_CURRENT_LIST_DIR}/model.cpp)

# Link the library to the executable
target_link_libraries(main 
# pico-tflmicro-lib
CMSISDSP
pico_stdlib
hardware_adc 
hardware_dma 
hardware_irq)


# Include necessary headers for the executable
# target_include_directories(main PRIVATE
#   ${CMAKE_CURRENT_LIST_DIR}/src/
# )


# Enable USB output and other standard features
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# Add the necessary build output for the target
pico_add_extra_outputs(main)