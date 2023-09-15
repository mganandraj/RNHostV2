# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("rnx-host")

add_library(rnx-host
        SHARED
        JJSBundle.cpp
        OfficeExecutorFactory.cpp
        OfficeExecutor.cpp
        JOfficeExecutorObserver.cpp
        OnLoad.cpp
)

target_compile_definitions(rnx-host PRIVATE MS_TARGET_ANDROID)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(rnx-host ReactAndroid::fabricjni)
target_link_libraries(rnx-host ReactAndroid::folly_runtime)
target_link_libraries(rnx-host ReactAndroid::glog)
target_link_libraries(rnx-host ReactAndroid::hermes_executor)
#target_link_libraries(rnx-host ReactAndroid::jscexecutor)
target_link_libraries(rnx-host ReactAndroid::jsi)
target_link_libraries(rnx-host ReactAndroid::jsinspector)
target_link_libraries(rnx-host ReactAndroid::react_codegen_rncore)
target_link_libraries(rnx-host ReactAndroid::react_debug)
target_link_libraries(rnx-host ReactAndroid::react_nativemodule_core)
target_link_libraries(rnx-host ReactAndroid::react_newarchdefaults)
target_link_libraries(rnx-host ReactAndroid::react_render_animations)
target_link_libraries(rnx-host ReactAndroid::react_render_componentregistry)
target_link_libraries(rnx-host ReactAndroid::react_render_core)
target_link_libraries(rnx-host ReactAndroid::react_render_debug)
target_link_libraries(rnx-host ReactAndroid::react_render_graphics)
target_link_libraries(rnx-host ReactAndroid::react_render_imagemanager)
target_link_libraries(rnx-host ReactAndroid::react_render_mapbuffer)
target_link_libraries(rnx-host ReactAndroid::react_render_mounting)
target_link_libraries(rnx-host ReactAndroid::react_render_scheduler)
target_link_libraries(rnx-host ReactAndroid::react_render_uimanager)
target_link_libraries(rnx-host ReactAndroid::reactnativejni)
target_link_libraries(rnx-host ReactAndroid::rrc_image)
target_link_libraries(rnx-host ReactAndroid::rrc_root)
target_link_libraries(rnx-host ReactAndroid::rrc_view)
target_link_libraries(rnx-host ReactAndroid::runtimeexecutor)
target_link_libraries(rnx-host ReactAndroid::turbomodulejsijni)
target_link_libraries(rnx-host ReactAndroid::yoga)

find_package(hermes-engine REQUIRED CONFIG)
target_link_libraries(rnx-host hermes-engine::libhermes)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(rnx-host fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(rnx-host ${log-lib})
