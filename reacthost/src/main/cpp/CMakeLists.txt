# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("reacthost")

add_library(reacthost
        SHARED
        AssetJSBundle_Android.cpp
        BundleServerJSBundle.cpp
        InstanceFactory_Android.cpp
        ReactInstanceAndroid.cpp
        JInstanceCreatedCallback.cpp
        JReactHost.cpp
        JReactHostStatics.cpp
        JReactInstance.cpp
        JReactViewHost.cpp
        JReactOptions.cpp
        JReactViewOptions.cpp
        JReactViewInstance.cpp
        JSBundle.cpp
        JSBundle_Android.cpp
        MsoUtils.cpp
        OnLoad.cpp
        ReactHost.cpp
)

# TODO:: Why is this necessary ?
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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
