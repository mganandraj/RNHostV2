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

find_library(log-lib log)

target_link_libraries(reactrekadroid ${log-lib})

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(reactrekadroid officeplat::plat)

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

find_package(mso REQUIRED CONFIG)
target_compile_definitions(reactrekadroid PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reactrekadroid PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reactrekadroid PRIVATE USE_OPENSOUCE_MSO)
target_link_libraries(reactrekadroid mso::activeObject)
target_link_libraries(reactrekadroid mso::compilerAdapters)
target_link_libraries(reactrekadroid mso::comUtil)
target_link_libraries(reactrekadroid mso::cppExtensions)
target_link_libraries(reactrekadroid mso::crash)
target_link_libraries(reactrekadroid mso::debugAssertApi)
target_link_libraries(reactrekadroid mso::dispatchQueue)
target_link_libraries(reactrekadroid mso::errorCode)
target_link_libraries(reactrekadroid mso::eventWaitHandle)
target_link_libraries(reactrekadroid mso::functional)
target_link_libraries(reactrekadroid mso::future)
target_link_libraries(reactrekadroid mso::guid)
target_link_libraries(reactrekadroid mso::memoryApi)
target_link_libraries(reactrekadroid mso::motifCpp)
target_link_libraries(reactrekadroid mso::oacr)
target_link_libraries(reactrekadroid mso::oacr_posix)
target_link_libraries(reactrekadroid mso::object)
target_link_libraries(reactrekadroid mso::platformAdapters)
target_link_libraries(reactrekadroid mso::platform_posix)
target_link_libraries(reactrekadroid mso::smartPtr)
target_link_libraries(reactrekadroid mso::span)
target_link_libraries(reactrekadroid mso::tagUtils)
target_link_libraries(reactrekadroid mso::typeTraits)