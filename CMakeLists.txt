cmake_minimum_required(VERSION 3.11)

project(inotify-cpp)

#enable_testing()

set(CMAKE_CXX_STANDARD 11)

set(
  CMAKE_CXX_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -g3 -O0"
)

if(${INOTIFY_CLANG_COVERAGE})
    set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
endif()

if(${INOTIFY_GCC_COVERAGE})
    set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(source)
# TODO drop boost and enable tests add_subdirectory(test)
#add_subdirectory(test)
