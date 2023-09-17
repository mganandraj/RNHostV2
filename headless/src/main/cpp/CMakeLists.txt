# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

project("headless")

add_library(headless SHARED headless.cpp JSHeadlessRuntimeImpl.cpp)

find_library(log-lib log)
target_link_libraries(headless ${log-lib})

find_package(mso REQUIRED CONFIG)
target_compile_definitions(headless PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(headless PRIVATE MS_TARGET_POSIX)
target_link_libraries(headless mso::activeObject)
target_link_libraries(headless mso::compilerAdapters)
target_link_libraries(headless mso::comUtil)
target_link_libraries(headless mso::cppExtensions)
target_link_libraries(headless mso::crash)
target_link_libraries(headless mso::debugAssertApi)
target_link_libraries(headless mso::dispatchQueue)
target_link_libraries(headless mso::errorCode)
target_link_libraries(headless mso::eventWaitHandle)
target_link_libraries(headless mso::functional)
target_link_libraries(headless mso::future)
target_link_libraries(headless mso::guid)
target_link_libraries(headless mso::memoryApi)
target_link_libraries(headless mso::motifCpp)
target_link_libraries(headless mso::oacr)
target_link_libraries(headless mso::oacr_posix)
target_link_libraries(headless mso::object)
target_link_libraries(headless mso::platformAdapters)
target_link_libraries(headless mso::platform_posix)
target_link_libraries(headless mso::smartPtr)
target_link_libraries(headless mso::span)
target_link_libraries(headless mso::tagUtils)
target_link_libraries(headless mso::typeTraits)

find_package(fbjni REQUIRED CONFIG)
target_link_libraries(headless fbjni::fbjni)

find_package(reacthost REQUIRED CONFIG)
target_link_libraries(headless reacthost::reacthost)
