# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project("sdxruntime")

add_library(sdxruntime
        SHARED
        AssetJSBundle_Android.cpp
        BundleServerJSBundle.cpp
        InstanceFactory_Android.cpp
        JSBundle.cpp
        JSBundle_Android.cpp
        MsoUtils.cpp
        ReactHost.cpp
        PlatformJSBundles_android.cpp
        ReactInstanceAndroidDeferred.cpp
        ReactHostStatics.cpp
)

target_compile_definitions(sdxruntime PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(sdxruntime PRIVATE MS_TARGET_POSIX)
target_compile_definitions(sdxruntime PRIVATE USE_OPENSOUCE_MSO)

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(sdxruntime officeplat::plat)

find_package(mso20android REQUIRED CONFIG)
target_link_libraries(sdxruntime mso20android::mso20android)

# Header only dependency
find_package(fbjni REQUIRED CONFIG)
target_link_libraries(sdxruntime fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(sdxruntime ${log-lib})

find_package(precomp REQUIRED CONFIG)
target_link_libraries(sdxruntime precomp::precomp)

add_library(officereacthost
        SHARED
        ReactInstanceAndroid.cpp
        )

target_compile_definitions(officereacthost PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(officereacthost PRIVATE MS_TARGET_POSIX)
target_compile_definitions(officereacthost PRIVATE USE_OPENSOUCE_MSO)

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(officereacthost officeplat::plat)

find_package(mso20android REQUIRED CONFIG)
target_link_libraries(officereacthost mso20android::mso20android)

# Depends on RN only for headers
find_package(ReactAndroid REQUIRED CONFIG)
target_link_libraries(officereacthost ReactAndroid::runtimeexecutor)
target_link_libraries(officereacthost ReactAndroid::jsi)
target_link_libraries(officereacthost ReactAndroid::react_nativemodule_core)
target_link_libraries(officereacthost ReactAndroid::turbomodulejsijni)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(officereacthost fbjni::fbjni)

find_library(log-lib log)
target_link_libraries(officereacthost ${log-lib})

find_package(precomp REQUIRED CONFIG)
target_link_libraries(officereacthost precomp::precomp)
