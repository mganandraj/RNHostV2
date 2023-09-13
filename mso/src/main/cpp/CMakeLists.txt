# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("mso")

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