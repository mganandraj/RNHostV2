# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("reactreka")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(reactreka SHARED
        OnLoad.cpp
        JRekaBridgeOptions.cpp
        RekaNativeModuleProxy.cpp
        )

find_library(log-lib log)

target_link_libraries(reactreka ${log-lib})

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(reactreka ReactAndroid::fabricjni)
target_link_libraries(reactreka ReactAndroid::folly_runtime)
target_link_libraries(reactreka ReactAndroid::glog)
target_link_libraries(reactreka ReactAndroid::hermes_executor)
#target_link_libraries(reactreka ReactAndroid::jscexecutor)
target_link_libraries(reactreka ReactAndroid::jsi)
target_link_libraries(reactreka ReactAndroid::jsinspector)
target_link_libraries(reactreka ReactAndroid::react_codegen_rncore)
target_link_libraries(reactreka ReactAndroid::react_debug)
target_link_libraries(reactreka ReactAndroid::react_nativemodule_core)
target_link_libraries(reactreka ReactAndroid::react_newarchdefaults)
target_link_libraries(reactreka ReactAndroid::react_render_animations)
target_link_libraries(reactreka ReactAndroid::react_render_componentregistry)
target_link_libraries(reactreka ReactAndroid::react_render_core)
target_link_libraries(reactreka ReactAndroid::react_render_debug)
target_link_libraries(reactreka ReactAndroid::react_render_graphics)
target_link_libraries(reactreka ReactAndroid::react_render_imagemanager)
target_link_libraries(reactreka ReactAndroid::react_render_mapbuffer)
target_link_libraries(reactreka ReactAndroid::react_render_mounting)
target_link_libraries(reactreka ReactAndroid::react_render_scheduler)
target_link_libraries(reactreka ReactAndroid::react_render_uimanager)
target_link_libraries(reactreka ReactAndroid::reactnativejni)
target_link_libraries(reactreka ReactAndroid::rrc_image)
target_link_libraries(reactreka ReactAndroid::rrc_root)
target_link_libraries(reactreka ReactAndroid::rrc_view)
target_link_libraries(reactreka ReactAndroid::runtimeexecutor)
target_link_libraries(reactreka ReactAndroid::turbomodulejsijni)
target_link_libraries(reactreka ReactAndroid::yoga)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(reactreka fbjni::fbjni)

find_package(reka REQUIRED CONFIG)
target_link_libraries(reactreka reka::reka)

find_package(json REQUIRED CONFIG)
target_link_libraries(reactreka json::json)

find_package(mso REQUIRED CONFIG)
target_compile_definitions(reactreka PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reactreka PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reactreka PRIVATE USE_OPENSOUCE_MSO)
target_link_libraries(reactreka mso::activeObject)
target_link_libraries(reactreka mso::compilerAdapters)
target_link_libraries(reactreka mso::comUtil)
target_link_libraries(reactreka mso::cppExtensions)
target_link_libraries(reactreka mso::crash)
target_link_libraries(reactreka mso::debugAssertApi)
target_link_libraries(reactreka mso::dispatchQueue)
target_link_libraries(reactreka mso::errorCode)
target_link_libraries(reactreka mso::eventWaitHandle)
target_link_libraries(reactreka mso::functional)
target_link_libraries(reactreka mso::future)
target_link_libraries(reactreka mso::guid)
target_link_libraries(reactreka mso::memoryApi)
target_link_libraries(reactreka mso::motifCpp)
target_link_libraries(reactreka mso::oacr)
target_link_libraries(reactreka mso::oacr_posix)
target_link_libraries(reactreka mso::object)
target_link_libraries(reactreka mso::platformAdapters)
target_link_libraries(reactreka mso::platform_posix)
target_link_libraries(reactreka mso::smartPtr)
target_link_libraries(reactreka mso::span)
target_link_libraries(reactreka mso::tagUtils)
target_link_libraries(reactreka mso::typeTraits)