# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("rnhostv2_2")

# set(FBJNI_ROOT /Users/anandrag/code/fbjni)
set(FBJNI_ROOT D:/github/fbjni)
file(GLOB fbjni_SOURCES
        ${FBJNI_ROOT}/cxx/fbjni/*.cpp
        ${FBJNI_ROOT}/cxx/fbjni/detail/*.cpp
        ${FBJNI_ROOT}/cxx/lyra/*.cpp
        )

add_library(fbjni
        SHARED
        ${fbjni_SOURCES})

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

# target_include_directories(rnhostv2_2 PRIVATE /Users/anandrag/code/RNHostV2/app/src/main/cpp)
target_include_directories(rnhostv2_2 PRIVATE D:/github/RNHostv2/app/app/src/main/cpp)

include_directories(${FBJNI_ROOT}/cxx/)
include_directories(D:/github/RNHostv2/app/src/main/cpp) # for sal.h

find_library(log-lib log)

# Office stuff
add_compile_definitions(MS_TARGET_ANDROID)
# target_compile_definitions(rnhostv2_2 PRIVATE MS_TARGET_ANDROID)

# MSO - Start

set(MSO_LIBLET_PLATFORM ANDROID)
add_compile_options ( -Wno-deprecated-declarations )
add_compile_options ( -Wno-inconsistent-missing-override )
add_compile_options ( -Wno-self-assign-overloaded )
add_compile_options ( -Wno-unused-function )
add_compile_options ( -Wno-error )
add_compile_options ( -Wnonportable-include-path )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")
include(liblet)

add_subdirectory(external)
add_subdirectory(libs)

# MSO - End

target_compile_definitions(rnhostv2_2 PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(rnhostv2_2 PRIVATE MS_TARGET_POSIX)

target_link_libraries(fbjni ${log-lib})
target_link_libraries(rnhostv2_2 fbjni)

target_link_libraries(rnhostv2_2 dispatchQueue)
target_link_libraries(rnhostv2_2 errorCode)
target_link_libraries(rnhostv2_2 future)
target_link_libraries(rnhostv2_2 smartPtr)
target_link_libraries(rnhostv2_2 activeObject)

target_link_libraries(rnhostv2_2 ${log-lib})
