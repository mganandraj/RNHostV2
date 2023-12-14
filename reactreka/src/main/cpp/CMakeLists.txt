# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("reactreka")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(reactrekadroid SHARED
        OnLoad.cpp
        JRekaBridgeOptions.cpp
        RekaNativeModuleProxy.cpp
        )

target_compile_definitions(reactrekadroid PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reactrekadroid PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reactrekadroid PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(reactrekadroid ${log-lib})

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(reactrekadroid officeplat::plat)
target_link_libraries(reactrekadroid officeplat::mso20android)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(reactrekadroid ReactAndroid::fabricjni)
target_link_libraries(reactrekadroid ReactAndroid::folly_runtime)
target_link_libraries(reactrekadroid ReactAndroid::glog)
target_link_libraries(reactrekadroid ReactAndroid::hermes_executor)
#target_link_libraries(reactrekadroid ReactAndroid::jscexecutor)
target_link_libraries(reactrekadroid ReactAndroid::jsi)
target_link_libraries(reactrekadroid ReactAndroid::jsinspector)
target_link_libraries(reactrekadroid ReactAndroid::react_codegen_rncore)
target_link_libraries(reactrekadroid ReactAndroid::react_debug)
target_link_libraries(reactrekadroid ReactAndroid::react_nativemodule_core)
target_link_libraries(reactrekadroid ReactAndroid::react_newarchdefaults)
target_link_libraries(reactrekadroid ReactAndroid::react_render_animations)
target_link_libraries(reactrekadroid ReactAndroid::react_render_componentregistry)
target_link_libraries(reactrekadroid ReactAndroid::react_render_core)
target_link_libraries(reactrekadroid ReactAndroid::react_render_debug)
target_link_libraries(reactrekadroid ReactAndroid::react_render_graphics)
target_link_libraries(reactrekadroid ReactAndroid::react_render_imagemanager)
target_link_libraries(reactrekadroid ReactAndroid::react_render_mapbuffer)
target_link_libraries(reactrekadroid ReactAndroid::react_render_mounting)
target_link_libraries(reactrekadroid ReactAndroid::react_render_scheduler)
target_link_libraries(reactrekadroid ReactAndroid::react_render_uimanager)
target_link_libraries(reactrekadroid ReactAndroid::reactnativejni)
target_link_libraries(reactrekadroid ReactAndroid::rrc_image)
target_link_libraries(reactrekadroid ReactAndroid::rrc_root)
target_link_libraries(reactrekadroid ReactAndroid::rrc_view)
target_link_libraries(reactrekadroid ReactAndroid::runtimeexecutor)
target_link_libraries(reactrekadroid ReactAndroid::turbomodulejsijni)
target_link_libraries(reactrekadroid ReactAndroid::yoga)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(reactrekadroid fbjni::fbjni)

find_package(reka REQUIRED CONFIG)
target_link_libraries(reactrekadroid reka::reka)

find_package(json REQUIRED CONFIG)
target_link_libraries(reactrekadroid json::json)
