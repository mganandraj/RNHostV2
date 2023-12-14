# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project("reacthostapp")

add_library(reacthostapp
        SHARED
        AwesomeRekaService.cpp
        MainActivity.cpp
        OnLoad.cpp
        ORNHostKtNativeOptionsFragment.cpp
)

target_compile_definitions(reacthostapp PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reacthostapp PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reacthostapp PRIVATE USE_OPENSOUCE_MSO)

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(reacthostapp officeplat::plat)
target_link_libraries(reacthostapp officeplat::mso20android)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(reacthostapp ReactAndroid::jsi)
target_link_libraries(reacthostapp ReactAndroid::turbomodulejsijni)
target_link_libraries(reacthostapp ReactAndroid::react_nativemodule_core)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(reacthostapp fbjni::fbjni)

find_package(sdxruntime REQUIRED CONFIG)
target_link_libraries(reacthostapp sdxruntime::sdxruntime)
target_link_libraries(reacthostapp sdxruntime::officereacthost)

find_package(headless REQUIRED CONFIG)
target_link_libraries(reacthostapp headless::headless)

find_library(log-lib log)
target_link_libraries(reacthostapp ${log-lib})

find_package(json REQUIRED CONFIG)
target_link_libraries(reacthostapp json::json)

find_package(reka REQUIRED CONFIG)
target_link_libraries(reacthostapp reka::reka)
