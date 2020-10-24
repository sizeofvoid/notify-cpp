project(InotifyUnitTest)

find_package(Threads REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "BSD")

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(event_handler_unit_test main.cpp event_handler_test.cpp)
target_link_libraries(
        event_handler_unit_test
        PUBLIC notify-cpp-shared stdc++fs Threads::Threads ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(event_handler_unit_test PRIVATE DOCTEST_CONFIG_DOUBLE_STRINGIFY=1)
target_include_directories(event_handler_unit_test PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include/")

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
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/")

add_test(NAME event_handler_unit_test  COMMAND event_handler_unit_test)
add_test(NAME inotify_unit_test COMMAND inotify_unit_test)

add_custom_command(TARGET fanotify_unit_test POST_BUILD
    COMMAND sudo setcap cap_sys_admin+ep $<TARGET_FILE:fanotify_unit_test>
    COMMENT "fanotify  needs cap_sys_admin to run"
)
add_test(NAME fanotify_unit_test COMMAND fanotify_unit_test)
else()
endif()
