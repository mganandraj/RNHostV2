# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

project("json")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(json STATIC
        jsonReader.cpp
        jsonStorage.cpp
        jsonWriter.cpp
        writer.cpp
        )

target_compile_definitions(json PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(json PRIVATE MS_TARGET_POSIX)
target_compile_definitions(json PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(json ${log-lib})

find_package(officeplat REQUIRED CONFIG)

target_link_libraries(json officeplat::plat)
target_link_libraries(json officeplat::mso20android)
