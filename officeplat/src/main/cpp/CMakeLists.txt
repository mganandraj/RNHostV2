# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)

project("officeplat")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(plat SHARED
        OfficeAssetApi.cpp
        wstring_inst.cpp
)

target_compile_definitions(plat PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(plat PRIVATE MS_TARGET_POSIX)
target_compile_definitions(plat PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(plat ${log-lib})
