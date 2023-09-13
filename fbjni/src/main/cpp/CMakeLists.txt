# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("fbjni")

file(GLOB fbjni_SOURCES
        ${FBJNI_ROOT}/cxx/fbjni/*.cpp
        ${FBJNI_ROOT}/cxx/fbjni/detail/*.cpp
        ${FBJNI_ROOT}/cxx/lyra/*.cpp
        )

add_library(fbjni
        SHARED
        ${fbjni_SOURCES})

target_include_directories(fbjni PRIVATE ${FBJNI_ROOT}/cxx)

find_library(log-lib log)

target_link_libraries(fbjni ${log-lib})
