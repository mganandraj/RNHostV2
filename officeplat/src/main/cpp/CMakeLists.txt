# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)

project("officeplat")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(plat STATIC
        stub.cpp 
        wstring_inst.cpp
        stringcore/localeSupport_android.cpp
        stringcore/stringComparison.cpp
        stringcore/stringParsing.cpp
        stringcore/stringConversion.cpp
        stringcore/stringTraits.cpp
        stringcore/stringUtils.cpp
)

target_compile_definitions(plat PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(plat PRIVATE MS_TARGET_POSIX)
target_compile_definitions(plat PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(plat ${log-lib})

find_package(mso REQUIRED CONFIG)
target_link_libraries(plat mso::compilerAdapters)
target_link_libraries(plat mso::guid)
target_link_libraries(plat mso::memoryApi)
target_link_libraries(plat mso::oacr)
target_link_libraries(plat mso::oacr_posix)
target_link_libraries(plat mso::platformAdapters)
target_link_libraries(plat mso::platform_posix)
target_link_libraries(plat mso::crash)
target_link_libraries(plat mso::debugAssertApi)
target_link_libraries(plat mso::smartPtr)
target_link_libraries(plat mso::tagUtils)
target_link_libraries(plat mso::typeTraits)
