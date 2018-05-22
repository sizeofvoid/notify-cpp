set(LIB_NAME notify-cpp)
set(LIB_SRCS
    notify.cpp
    notifier_builder.cpp
    event.cpp
    notification.cpp
    file_system_event.cpp
    fanotify.cpp
    inotify.cpp)

add_library(${LIB_NAME} ${LIB_SRCS})
target_include_directories(
  ${LIB_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

install(
  TARGETS ${LIB_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h*"
)
