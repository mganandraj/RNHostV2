# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)

project("rnxreacthost")

add_library(rnxreacthost
        SHARED
        JJSBundle.cpp
        WrapperJSExecutorFactory.cpp
        WrapperJSExecutor.cpp
        JJSExecutorObserver.cpp
        OnLoad.cpp
)

target_compile_definitions(rnxreacthost PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(rnxreacthost PRIVATE USE_OPENSOUCE_MSO)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(rnxreacthost ReactAndroid::fabricjni)
target_link_libraries(rnxreacthost ReactAndroid::folly_runtime)
target_link_libraries(rnxreacthost ReactAndroid::glog)
target_link_libraries(rnxreacthost ReactAndroid::hermes_executor)
#target_link_libraries(rnxreacthost ReactAndroid::jscexecutor)
target_link_libraries(rnxreacthost ReactAndroid::jsi)
target_link_libraries(rnxreacthost ReactAndroid::jsinspector)
target_link_libraries(rnxreacthost ReactAndroid::react_codegen_rncore)
target_link_libraries(rnxreacthost ReactAndroid::react_debug)
target_link_libraries(rnxreacthost ReactAndroid::react_nativemodule_core)
target_link_libraries(rnxreacthost ReactAndroid::react_newarchdefaults)
target_link_libraries(rnxreacthost ReactAndroid::react_render_animations)
target_link_libraries(rnxreacthost ReactAndroid::react_render_componentregistry)
target_link_libraries(rnxreacthost ReactAndroid::react_render_core)
target_link_libraries(rnxreacthost ReactAndroid::react_render_debug)
target_link_libraries(rnxreacthost ReactAndroid::react_render_graphics)
target_link_libraries(rnxreacthost ReactAndroid::react_render_imagemanager)
target_link_libraries(rnxreacthost ReactAndroid::react_render_mapbuffer)
target_link_libraries(rnxreacthost ReactAndroid::react_render_mounting)
target_link_libraries(rnxreacthost ReactAndroid::react_render_scheduler)
target_link_libraries(rnxreacthost ReactAndroid::react_render_uimanager)
target_link_libraries(rnxreacthost ReactAndroid::reactnativejni)
target_link_libraries(rnxreacthost ReactAndroid::rrc_image)
target_link_libraries(rnxreacthost ReactAndroid::rrc_root)
target_link_libraries(rnxreacthost ReactAndroid::rrc_view)
target_link_libraries(rnxreacthost ReactAndroid::runtimeexecutor)
target_link_libraries(rnxreacthost ReactAndroid::turbomodulejsijni)
target_link_libraries(rnxreacthost ReactAndroid::yoga)

find_package(hermes-engine REQUIRED CONFIG)
target_link_libraries(rnxreacthost hermes-engine::libhermes)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(rnxreacthost fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(rnxreacthost ${log-lib})
