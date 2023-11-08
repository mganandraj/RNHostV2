# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

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

find_library(log-lib log)
target_link_libraries(reka ${log-lib})

find_package(mso REQUIRED CONFIG)
target_compile_definitions(reka PRIVATE MS_TARGET_ANDROID)
target_compile_definitions(reka PRIVATE MS_TARGET_POSIX)
target_compile_definitions(reka PRIVATE USE_OPENSOUCE_MSO)
target_link_libraries(reka mso::activeObject)
target_link_libraries(reka mso::compilerAdapters)
target_link_libraries(reka mso::comUtil)
target_link_libraries(reka mso::cppExtensions)
target_link_libraries(reka mso::crash)
target_link_libraries(reka mso::debugAssertApi)
target_link_libraries(reka mso::dispatchQueue)
target_link_libraries(reka mso::errorCode)
target_link_libraries(reka mso::eventWaitHandle)
target_link_libraries(reka mso::functional)
target_link_libraries(reka mso::future)
target_link_libraries(reka mso::guid)
target_link_libraries(reka mso::memoryApi)
target_link_libraries(reka mso::motifCpp)
target_link_libraries(reka mso::oacr)
target_link_libraries(reka mso::oacr_posix)
target_link_libraries(reka mso::object)
target_link_libraries(reka mso::platformAdapters)
target_link_libraries(reka mso::platform_posix)
target_link_libraries(reka mso::smartPtr)
target_link_libraries(reka mso::span)
target_link_libraries(reka mso::tagUtils)
target_link_libraries(reka mso::typeTraits)

find_package(json REQUIRED CONFIG)
target_link_libraries(reka json::json)