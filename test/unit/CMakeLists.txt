project(InotifyUnitTest)

find_package(Threads REQUIRED)

add_executable(event_handler_unit_test main.cpp event_handler_test.cpp)
target_link_libraries(
        event_handler_unit_test
        PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(event_handler_unit_test PRIVATE DOCTEST_CONFIG_DOUBLE_STRINGIFY=1)
target_include_directories(event_handler_unit_test PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include/"
        "${Boost_INCLUDE_DIRS}")

add_executable(inotify_unit_test main.cpp inotify_controller_test.cpp)
target_link_libraries(
  inotify_unit_test
  PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(inotify_unit_test PRIVATE DOCTEST_CONFIG_DOUBLE_STRINGIFY=1)
target_include_directories(inotify_unit_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/")

add_executable(fanotify_unit_test main.cpp fanotify_controller_test.cpp)
target_link_libraries(
  fanotify_unit_test
  PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(fanotify_unit_test PRIVATE DOCTEST_CONFIG_DOUBLE_STRINGIFY=1)
target_include_directories(fanotify_unit_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/"
    "${Boost_INCLUDE_DIRS}")

add_test(NAME event_handler_unit_test  COMMAND event_handler_unit_test)
add_test(NAME inotify_unit_test COMMAND inotify_unit_test)
add_test(NAME fanotify_unit_test COMMAND fanotify_unit_test)
