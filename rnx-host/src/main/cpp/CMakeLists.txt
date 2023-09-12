# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set(CMAKE_CXX_STANDARD 20)

project("rnx-host")

add_library(rnx-host
        SHARED
        native-lib.cpp
#        OfficeExecutorFactory.cpp
#        OfficeExecutor.cpp
#        JOfficeExecutorObserver.cpp
#        OnLoad.cpp
)

find_library(log-lib log)

target_link_libraries(rnx-host ${log-lib})
