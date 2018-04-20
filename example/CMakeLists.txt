cmake_minimum_required(VERSION 2.8)
project(InotifyExample)

find_package(Threads)

find_library(
  INOTIFY_CPP_LIBRARY inotify-cpp
  HINTS ${CMAKE_SOURCE_DIR}/../source
)

add_executable(inotify_example main.cpp)
target_link_libraries(
  inotify_example
  PUBLIC ${INOTIFY_CPP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(
  inotify_example
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
