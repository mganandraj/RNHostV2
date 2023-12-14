# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)

project("precomp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# TODO : Figure out how to use INTERFACE libraries
add_library(precomp STATIC stub.cpp)

target_compile_definitions(precomp INTERFACE MS_TARGET_ANDROID)
target_compile_definitions(precomp INTERFACE MS_TARGET_POSIX)
target_compile_definitions(precomp INTERFACE USE_OPENSOUCE_MSO)

