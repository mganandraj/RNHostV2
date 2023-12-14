# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

project("reka")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(reka STATIC
        JsonReader.cpp
        JsonWriter.cpp
        PropertyBag.cpp
        reka.cpp
        RekaCallContext.cpp
        RekaContext.cpp
        RekaContextProxy.cpp
        RekaErrorProvider.cpp
        RekaInstance.cpp
        RekaQueueService.cpp
        RekaReader.cpp
        RekaServiceProviderRegistration.cpp
        RekaServiceRegistration.cpp
        RekaWriter.cpp
        StringConversion.cpp
)

target_compile_definitions(reka PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reka PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reka PRIVATE USE_OPENSOUCE_MSO)

find_library(log-lib log)
target_link_libraries(reka ${log-lib})

find_package(officeplat REQUIRED CONFIG)
target_link_libraries(reka officeplat::plat)
target_link_libraries(reka officeplat::mso20android)

find_package(json REQUIRED CONFIG)
target_link_libraries(reka json::json)