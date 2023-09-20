# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

project("json")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(json STATIC
        jsonReader.cpp
        jsonStorage.cpp
        jsonWriter.cpp
        writer.cpp
        stringcore/stringComparison.cpp
        stringcore/stringParsing.cpp
        stringcore/stringConversion.cpp
        stringcore/stringTraits.cpp
        stringcore/stringUtils.cpp
        )

target_include_directories(json PRIVATE D:/github/RNHostv2/json/src/main/cpp/wchar16)

find_library(log-lib log)
target_link_libraries(json ${log-lib})

find_package(mso REQUIRED CONFIG)
target_compile_definitions(json PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(json PRIVATE MS_TARGET_POSIX)
target_link_libraries(json mso::activeObject)
target_link_libraries(json mso::compilerAdapters)
target_link_libraries(json mso::comUtil)
target_link_libraries(json mso::cppExtensions)
target_link_libraries(json mso::crash)
target_link_libraries(json mso::debugAssertApi)
target_link_libraries(json mso::dispatchQueue)
target_link_libraries(json mso::errorCode)
target_link_libraries(json mso::eventWaitHandle)
target_link_libraries(json mso::functional)
target_link_libraries(json mso::future)
target_link_libraries(json mso::guid)
target_link_libraries(json mso::memoryApi)
target_link_libraries(json mso::motifCpp)
target_link_libraries(json mso::oacr)
target_link_libraries(json mso::oacr_posix)
target_link_libraries(json mso::object)
target_link_libraries(json mso::platformAdapters)
target_link_libraries(json mso::platform_posix)
target_link_libraries(json mso::smartPtr)
target_link_libraries(json mso::span)
target_link_libraries(json mso::tagUtils)
target_link_libraries(json mso::typeTraits)