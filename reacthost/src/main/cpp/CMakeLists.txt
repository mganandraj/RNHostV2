# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project("reacthost")

add_library(reacthost
        SHARED
        ReactInstanceAndroid.cpp
        AssetJSBundle_Android.cpp
        BundleServerJSBundle.cpp
        InstanceFactory_Android.cpp
        JSBundle.cpp
        JSBundle_Android.cpp
        MsoUtils.cpp
        ReactHost.cpp
        PlatformJSBundles_android.cpp
)

# TODO:: Why is this necessary ?
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(reacthost ReactAndroid::fabricjni)
target_link_libraries(reacthost ReactAndroid::folly_runtime)
target_link_libraries(reacthost ReactAndroid::glog)
target_link_libraries(reacthost ReactAndroid::hermes_executor)
#target_link_libraries(reacthost ReactAndroid::jscexecutor)
target_link_libraries(reacthost ReactAndroid::jsi)
target_link_libraries(reacthost ReactAndroid::jsinspector)
target_link_libraries(reacthost ReactAndroid::react_codegen_rncore)
target_link_libraries(reacthost ReactAndroid::react_debug)
target_link_libraries(reacthost ReactAndroid::react_nativemodule_core)
target_link_libraries(reacthost ReactAndroid::react_newarchdefaults)
target_link_libraries(reacthost ReactAndroid::react_render_animations)
target_link_libraries(reacthost ReactAndroid::react_render_componentregistry)
target_link_libraries(reacthost ReactAndroid::react_render_core)
target_link_libraries(reacthost ReactAndroid::react_render_debug)
target_link_libraries(reacthost ReactAndroid::react_render_graphics)
target_link_libraries(reacthost ReactAndroid::react_render_imagemanager)
target_link_libraries(reacthost ReactAndroid::react_render_mapbuffer)
target_link_libraries(reacthost ReactAndroid::react_render_mounting)
target_link_libraries(reacthost ReactAndroid::react_render_scheduler)
target_link_libraries(reacthost ReactAndroid::react_render_uimanager)
target_link_libraries(reacthost ReactAndroid::reactnativejni)
target_link_libraries(reacthost ReactAndroid::rrc_image)
target_link_libraries(reacthost ReactAndroid::rrc_root)
target_link_libraries(reacthost ReactAndroid::rrc_view)
target_link_libraries(reacthost ReactAndroid::runtimeexecutor)
target_link_libraries(reacthost ReactAndroid::turbomodulejsijni)
target_link_libraries(reacthost ReactAndroid::yoga)

find_package(mso REQUIRED CONFIG)
target_compile_definitions(reacthost PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reacthost PRIVATE MS_TARGET_POSIX)
target_link_libraries(reacthost mso::activeObject)
target_link_libraries(reacthost mso::compilerAdapters)
target_link_libraries(reacthost mso::comUtil)
target_link_libraries(reacthost mso::cppExtensions)
target_link_libraries(reacthost mso::crash)
target_link_libraries(reacthost mso::debugAssertApi)
target_link_libraries(reacthost mso::dispatchQueue)
target_link_libraries(reacthost mso::errorCode)
target_link_libraries(reacthost mso::eventWaitHandle)
target_link_libraries(reacthost mso::functional)
target_link_libraries(reacthost mso::future)
target_link_libraries(reacthost mso::guid)
target_link_libraries(reacthost mso::memoryApi)
target_link_libraries(reacthost mso::motifCpp)
target_link_libraries(reacthost mso::oacr)
target_link_libraries(reacthost mso::oacr_posix)
target_link_libraries(reacthost mso::object)
target_link_libraries(reacthost mso::platformAdapters)
target_link_libraries(reacthost mso::platform_posix)
target_link_libraries(reacthost mso::smartPtr)
target_link_libraries(reacthost mso::span)
target_link_libraries(reacthost mso::tagUtils)
target_link_libraries(reacthost mso::typeTraits)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(reacthost fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(reacthost ${log-lib})

find_package(json REQUIRED CONFIG)
target_link_libraries(reacthost json::json)

find_package(reka REQUIRED CONFIG)
target_link_libraries(reacthost reka::reka)

find_package(reactreka REQUIRED CONFIG)
target_link_libraries(reacthost reactreka::reactreka)
