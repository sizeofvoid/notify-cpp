project(InotifyUnitTest)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system unit_test_framework)

if (NOT DEFINED Boost_INCLUDE_DIRS OR NOT Boost_LIBRARIES)
    message(FATAL_ERROR "Missing boost feature")
endif()

add_executable(inotify_unit_test main.cpp inotify_controller_test.cpp)
target_link_libraries(
  inotify_unit_test
  PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(inotify_unit_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/"
    "${Boost_INCLUDE_DIRS}")

target_link_directories(inotify_unit_test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

add_executable(fanotify_unit_test main.cpp fanotify_controller_test.cpp)
target_link_libraries(
  fanotify_unit_test
  PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(fanotify_unit_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/"
    "${Boost_INCLUDE_DIRS}")

target_link_directories(fanotify_unit_test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})


add_test(NAME inotify_unit_test COMMAND inotify_unit_test)
add_test(NAME fanotify_unit_test COMMAND fanotify_unit_test)
