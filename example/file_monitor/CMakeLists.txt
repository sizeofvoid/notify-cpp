cmake_minimum_required(VERSION 3.8)

project(SimpleFileMonitorExample)
find_package(notify-cpp REQUIRED)
find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_REQUIRED_FLAGS -std=c++17)
include(CheckCXXSymbolExists)

# check c++17 filesystem feature
CHECK_CXX_SYMBOL_EXISTS(std::filesystem::path::preferred_separator
                        filesystem
                        CXX_FILESYSTEM)

if (NOT CXX_FILESYSTEM)
    message(FATAL_ERROR "Missing C++17 std::filesystem feature")
endif()

add_executable(file_monitor main.cpp)

target_include_directories(
  file_monitor
  PUBLIC ${NOTIFYCPP_INCLUDE_DIRS}
)

target_link_libraries(
  file_monitor
  PUBLIC ${NOTIFYCPP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} stdc++fs
)
