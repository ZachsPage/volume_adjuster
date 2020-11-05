cmake_minimum_required(VERSION 3.5)

if( NOT DEFINED PROJECT_NAME)
    message( FATAL_ERROR "Please pass CMake a project name with "
                         "'cmake -DPROJECT_NAME=...', exiting")
endif()

set(MIN_COMPS bootloader esp32 esptool_py spi_flash cxx)
set(MY_COMPS MyBluetooth MySerial main)
set(COMPONENTS ${MIN_COMPS} ${MY_COMPS})

# Use revision controlled file as sdk configuration
set(SDKCONFIG_DEFAULTS ${CMAKE_CURRENT_LIST_DIR}/esp_files/sdkconfig.defaults)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project( ${PROJECT_NAME}
        VERSION 0.0.0 
        LANGUAGE CXX )