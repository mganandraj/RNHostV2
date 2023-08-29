# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)


project("rnhostv2_2")

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
        native-lib.cpp
        JReactOptions.cpp
)

include_directories(${FBJNI_ROOT}/cxx/)

find_library(log-lib log)
target_link_libraries(fbjni ${log-lib})
target_link_libraries(rnhostv2_2 fbjni)
target_link_libraries(rnhostv2_2 ${log-lib})
