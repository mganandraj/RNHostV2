# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)

project("officeplat")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(plat SHARED
        OfficeAssetApi.cpp
        wstring_inst.cpp
)

target_compile_definitions(plat PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(plat PRIVATE MS_TARGET_POSIX)
target_compile_definitions(plat PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(plat ${log-lib})

add_library( mso20android SHARED
        stub.cpp
        stringcore/localeSupport_android.cpp
        stringcore/stringComparison.cpp
        stringcore/stringParsing.cpp
        stringcore/stringConversion.cpp
        stringcore/stringTraits.cpp
        stringcore/stringUtils.cpp
)

find_package(mso REQUIRED CONFIG)
target_link_libraries( mso20android -Wl,--whole-archive mso::activeObject -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::compilerAdapters -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::comUtil -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::cppExtensions -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::crash -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::debugAssertApi -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::dispatchQueue -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::errorCode -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::eventWaitHandle -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::functional -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::future -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::guid -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::memoryApi -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::motifCpp -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::oacr -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::oacr_posix -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::object -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::platformAdapters -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::platform_posix -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::smartPtr -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::span -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::tagUtils -Wl,--no-whole-archive)
target_link_libraries( mso20android -Wl,--whole-archive mso::typeTraits -Wl,--no-whole-archive)

target_compile_definitions( mso20android PRIVATE MS_TARGET_ANDROID)
target_compile_definitions( mso20android PRIVATE MS_TARGET_POSIX)
target_compile_definitions( mso20android PRIVATE USE_OPENSOUCE_MSO)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries( mso20android GTest::gtest_main)

# include(GoogleTest)
# gtest_discover_tests(hello_test)