cmake_minimum_required(VERSION 3.8)

project(notify-cpp)

include(GNUInstallDirs)

# Version setup
set(NOTIFYCPP_VERSION_MAJOR "1")
set(NOTIFYCPP_VERSION_MINOR "0")
set(NOTIFYCPP_VERSION_PATCH "0")
set(NOTIFYCPP_VERSION "${NOTIFYCPP_VERSION_MAJOR}.${NOTIFYCPP_VERSION_MINOR}.${NOTIFYCPP_VERSION_PATCH}")

option(ENABLE_SHARED_LIBS "Enable build and install shared libraries" ON)
option(ENABLE_STATIC_LIBS "Enable build and install static libraries" OFF)


## Set the build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" 
      CACHE STRING "Build configuration 'Release' or 'Debug'."
      FORCE )
endif()

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

set(NOTIFYCPP_HEADER
    include/notify-cpp/event.h
    include/notify-cpp/fanotify.h
    include/notify-cpp/file_system_event.h
    include/notify-cpp/inotify.h
    include/notify-cpp/notification.h
    include/notify-cpp/notifier_builder.h
    include/notify-cpp/notify.h)

set(NOTIFYCPP_SOURCES
    source/event.cpp
    source/fanotify.cpp
    source/file_system_event.cpp
    source/inotify.cpp
    source/notification.cpp
    source/notifier_builder.cpp
    source/notify.cpp)

# XXX readlink
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic " CACHE STRING "Set C++ Compiler Flags" FORCE)

foreach (TYPE IN ITEMS STATIC SHARED)
  if (ENABLE_${TYPE}_LIBS)
    string (TOLOWER "${TYPE}" type)
    add_library(notify-cpp-${type} ${TYPE} ${NOTIFYCPP_SOURCES} ${NOTIFYCPP_HEADER})
    set_target_properties(notify-cpp-${type} PROPERTIES
        VERSION ${NOTIFYCPP_VERSION}
        SOVERSION ${NOTIFYCPP_VERSION_MAJOR})

    target_include_directories(notify-cpp-${type} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/notify-cpp/>)

  #target_compile_options(notify-cpp-${type} PUBLIC
  #    $<$<CONFIG:RELEASE>:${}>
  #    $<$<CONFIG:DEBUG:${}>)
  endif()
endforeach()


if(ENABLE_SHARED_LIBS)
  install(TARGETS notify-cpp-shared
      RENAME notify-cpp
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(ENABLE_STATIC_LIBS)
    set_target_properties(notify-cpp-static PROPERTIES OUTPUT_NAME notify-cpp)
    install(TARGETS notify-cpp-static
        RENAME notify-cpp
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

# TODO drop boost and enable tests add_subdirectory(test)
enable_testing()
add_subdirectory(test)
