# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("rnhostv2_2")

add_library(rnhostv2_2
        SHARED
        AssetJSBundle_Android.cpp
        BundleServerJSBundle.cpp
        InstanceFactory_Android.cpp
        ReactInstanceAndroid.cpp
        JReactOptions.cpp
        JReactViewOptions.cpp
        JReactViewInstance.cpp
        JSBundle.cpp
        JSBundle_Android.cpp
        MsoUtils.cpp
        native-lib.cpp
        ReactHost.cpp
        MainActivity.cpp
)

# TODO:: Why is this necessary ?
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(mso REQUIRED CONFIG)
target_compile_definitions(rnhostv2_2 PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(rnhostv2_2 PRIVATE MS_TARGET_POSIX)
target_link_libraries(rnhostv2_2 mso::activeObject)
target_link_libraries(rnhostv2_2 mso::compilerAdapters)
target_link_libraries(rnhostv2_2 mso::comUtil)
target_link_libraries(rnhostv2_2 mso::cppExtensions)
target_link_libraries(rnhostv2_2 mso::crash)
target_link_libraries(rnhostv2_2 mso::debugAssertApi)
target_link_libraries(rnhostv2_2 mso::dispatchQueue)
target_link_libraries(rnhostv2_2 mso::errorCode)
target_link_libraries(rnhostv2_2 mso::eventWaitHandle)
target_link_libraries(rnhostv2_2 mso::functional)
target_link_libraries(rnhostv2_2 mso::future)
target_link_libraries(rnhostv2_2 mso::guid)
target_link_libraries(rnhostv2_2 mso::memoryApi)
target_link_libraries(rnhostv2_2 mso::motifCpp)
target_link_libraries(rnhostv2_2 mso::oacr)
target_link_libraries(rnhostv2_2 mso::oacr_posix)
target_link_libraries(rnhostv2_2 mso::object)
target_link_libraries(rnhostv2_2 mso::platformAdapters)
target_link_libraries(rnhostv2_2 mso::platform_posix)
target_link_libraries(rnhostv2_2 mso::smartPtr)
target_link_libraries(rnhostv2_2 mso::span)
target_link_libraries(rnhostv2_2 mso::tagUtils)
target_link_libraries(rnhostv2_2 mso::typeTraits)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(rnhostv2_2 fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(rnhostv2_2 ${log-lib})
