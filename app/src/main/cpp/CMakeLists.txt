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
)

# TODO:: Why is this necessary ?
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(mso REQUIRED CONFIG)
target_compile_definitions(reacthostapp PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reacthostapp PRIVATE MS_TARGET_POSIX)
target_link_libraries(reacthostapp mso::activeObject)
target_link_libraries(reacthostapp mso::compilerAdapters)
target_link_libraries(reacthostapp mso::comUtil)
target_link_libraries(reacthostapp mso::cppExtensions)
target_link_libraries(reacthostapp mso::crash)
target_link_libraries(reacthostapp mso::debugAssertApi)
target_link_libraries(reacthostapp mso::dispatchQueue)
target_link_libraries(reacthostapp mso::errorCode)
target_link_libraries(reacthostapp mso::eventWaitHandle)
target_link_libraries(reacthostapp mso::functional)
target_link_libraries(reacthostapp mso::future)
target_link_libraries(reacthostapp mso::guid)
target_link_libraries(reacthostapp mso::memoryApi)
target_link_libraries(reacthostapp mso::motifCpp)
target_link_libraries(reacthostapp mso::oacr)
target_link_libraries(reacthostapp mso::oacr_posix)
target_link_libraries(reacthostapp mso::object)
target_link_libraries(reacthostapp mso::platformAdapters)
target_link_libraries(reacthostapp mso::platform_posix)
target_link_libraries(reacthostapp mso::smartPtr)
target_link_libraries(reacthostapp mso::span)
target_link_libraries(reacthostapp mso::tagUtils)
target_link_libraries(reacthostapp mso::typeTraits)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(reacthostapp ReactAndroid::jsi)
target_link_libraries(reacthostapp ReactAndroid::turbomodulejsijni)
target_link_libraries(reacthostapp ReactAndroid::react_nativemodule_core)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(reacthostapp fbjni::fbjni)

find_package(reacthost REQUIRED CONFIG)
target_link_libraries(reacthostapp reacthost::reacthost)

find_package(headless REQUIRED CONFIG)
target_link_libraries(reacthostapp headless::headless)

find_library(log-lib log)
target_link_libraries(reacthostapp ${log-lib})

find_package(json REQUIRED CONFIG)
target_link_libraries(reacthostapp json::json)

find_package(reka REQUIRED CONFIG)
target_link_libraries(reacthostapp reka::reka)
