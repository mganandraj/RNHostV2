# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 17)

project("headless")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(headless SHARED headless.cpp JSHeadlessRuntimeImpl.cpp)

target_compile_definitions(headless PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(headless PRIVATE MS_TARGET_POSIX)
target_compile_definitions(headless PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(headless ${log-lib})

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(headless officeplat::plat)

find_package(mso20android REQUIRED CONFIG)
target_link_libraries(headless mso20android::mso20android)

find_package(precomp REQUIRED CONFIG)
target_link_libraries(headless precomp::precomp)

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(headless ReactAndroid::fabricjni)
target_link_libraries(headless ReactAndroid::folly_runtime)
target_link_libraries(headless ReactAndroid::glog)
target_link_libraries(headless ReactAndroid::hermes_executor)
#target_link_libraries(headless ReactAndroid::jscexecutor)
target_link_libraries(headless ReactAndroid::jsi)
target_link_libraries(headless ReactAndroid::jsinspector)
target_link_libraries(headless ReactAndroid::react_codegen_rncore)
target_link_libraries(headless ReactAndroid::react_debug)
target_link_libraries(headless ReactAndroid::react_nativemodule_core)
target_link_libraries(headless ReactAndroid::react_newarchdefaults)
target_link_libraries(headless ReactAndroid::react_render_animations)
target_link_libraries(headless ReactAndroid::react_render_componentregistry)
target_link_libraries(headless ReactAndroid::react_render_core)
target_link_libraries(headless ReactAndroid::react_render_debug)
target_link_libraries(headless ReactAndroid::react_render_graphics)
target_link_libraries(headless ReactAndroid::react_render_imagemanager)
target_link_libraries(headless ReactAndroid::react_render_mapbuffer)
target_link_libraries(headless ReactAndroid::react_render_mounting)
target_link_libraries(headless ReactAndroid::react_render_scheduler)
target_link_libraries(headless ReactAndroid::react_render_uimanager)
target_link_libraries(headless ReactAndroid::reactnativejni)
target_link_libraries(headless ReactAndroid::rrc_image)
target_link_libraries(headless ReactAndroid::rrc_root)
target_link_libraries(headless ReactAndroid::rrc_view)
target_link_libraries(headless ReactAndroid::runtimeexecutor)
target_link_libraries(headless ReactAndroid::turbomodulejsijni)
target_link_libraries(headless ReactAndroid::yoga)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(headless fbjni::fbjni)

find_package(sdxruntime REQUIRED CONFIG)
target_link_libraries(headless sdxruntime::sdxruntime)
