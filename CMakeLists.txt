cmake_minimum_required(VERSION 3.8)

project(notify-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_REQUIRED_FLAGS -std=c++17)
include(CheckCXXSymbolExists)


# check c++17 filesystem feature
CHECK_CXX_SYMBOL_EXISTS(std::filesystem::path::preferred_separator
                        filesystem
                        cxx_filesystem)

if (NOT cxx_filesystem)
    message(FATAL_ERROR "Missing C++17 std::filesystem feature")
endif()

add_subdirectory(source)

# TODO drop boost and enable tests add_subdirectory(test)
#enable_testing()
#add_subdirectory(test)
