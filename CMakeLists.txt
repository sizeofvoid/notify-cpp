cmake_minimum_required(VERSION 3.8)

project(notify-cpp)

include(GNUInstallDirs)

# Version setup
set(NOTIFYCPP_VERSION_MAJOR "0")
set(NOTIFYCPP_VERSION_MINOR "1")
set(NOTIFYCPP_VERSION_PATCH "1")
set(NOTIFYCPP_VERSION "${NOTIFYCPP_VERSION_MAJOR}.${NOTIFYCPP_VERSION_MINOR}.${NOTIFYCPP_VERSION_PATCH}")

option(ENABLE_SHARED_LIBS "Enable build and install shared libraries" ON)
option(ENABLE_STATIC_LIBS "Enable build and install static libraries" OFF)
option(ENABLE_EXAMPLES "Enable building of samples" OFF)
option(ENABLE_TEST "Enable build the tests" ON)


## Set the build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
      CACHE STRING "Build configuration 'Release' or 'Debug'."
      FORCE)
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
    include/notify-cpp/notify_controller.h
    include/notify-cpp/notify.h
    include/notify-cpp/internal/util.h)

set(NOTIFYCPP_SOURCES
    source/event.cpp
    source/fanotify.cpp
    source/file_system_event.cpp
    source/inotify.cpp
    source/notification.cpp
    source/notify_controller.cpp
    source/notify.cpp
    source/util.cpp)

# XXX readlink
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic " CACHE STRING "Set C++ Compiler Flags" FORCE)


# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/notify-cpp)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

foreach (TYPE IN ITEMS STATIC SHARED)
  if (ENABLE_${TYPE}_LIBS)
    string (TOLOWER "${TYPE}" type)
    add_library(notify-cpp-${type} ${TYPE} ${NOTIFYCPP_SOURCES} ${NOTIFYCPP_HEADER})
    set_property(TARGET notify-cpp-${type} PROPERTY CXX_STANDARD 17)
    set_property(TARGET notify-cpp-${type} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_features(notify-cpp-${type} PUBLIC cxx_std_17)

    set_target_properties(notify-cpp-${type} PROPERTIES
        VERSION ${NOTIFYCPP_VERSION}
        SOVERSION ${NOTIFYCPP_VERSION_MAJOR})


    target_include_directories(notify-cpp-${type} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/notify-cpp/>)

  endif()
endforeach()

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE notify-cpp)

# Create the notify-cppConfig.cmake and notify-cppConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(notify-cppConfig.cmake.in
  "${PROJECT_BINARY_DIR}/notify-cppConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${NOTIFYCPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(notify-cppConfig.cmake.in
  "${PROJECT_BINARY_DIR}/notify-cppConfig.cmake" @ONLY)

# ... for both
configure_file(notify-cppConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/notify-cppConfigVersion.cmake" @ONLY)



if(ENABLE_SHARED_LIBS)
  install(TARGETS notify-cpp-shared
    RENAME notify-cpp
    EXPORT notify-cppTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
 # Install the export set for use with the install-tree
  install(EXPORT notify-cppTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
    FILE notify-cppTargets.cmake
    COMPONENT dev)
endif()

if(ENABLE_STATIC_LIBS)
    set_target_properties(notify-cpp-static PROPERTIES OUTPUT_NAME notify-cpp)
    install(TARGETS notify-cpp-static
        RENAME notify-cpp
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

# install incldues
install(DIRECTORY include/notify-cpp
    DESTINATION include)

# Install the FooBarConfig.cmake and FooBarConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/notify-cppConfig.cmake"
  "${PROJECT_BINARY_DIR}/notify-cppConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

if (ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if(ENABLE_EXAMPLES)
  add_subdirectory(example/file_monitor_local)
endif()
