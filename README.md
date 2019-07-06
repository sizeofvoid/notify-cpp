# notify-cpp - A C++17 interface for linux monitoring filesystem events
[![Build Status](https://travis-ci.org/sizeofvoid/notify-cpp.svg?branch=master)](https://travis-ci.org/sizeofvoid/notify-cpp)
===========

__notify-cpp__ is a C++17 wrapper for linux fanotify and inotify. It lets you
watch for filesystem events on your filesystem tree. It's based on the work of
__erikzenker/inotify-cpp__.

## Build Library

CMake build option:

- Enable build and install shared libraries. Default: On
  - `-DENABLE_SHARED_LIBS=OFF`
- Enable build and install static libraries. Default: Off
  - `-DENABLE_STATIC_LIBS=ON`
- Enable build the tests. Default: On. Depend on Boost test
  - `-DENABLE_TEST=OFF`

```bash

# Configure
mkdir build && cd bulid
cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ \
      -DDENABLE_STATIC_LIBS=ON \
      ..

# Build
make

# Run tests
make test

# Install the library
make install
```

## Dependencies
 + C++ 17 Compiler
 + CMake 3.8
 + linux 2.6.13

## Licence
MIT

## Author
Initially written by Erik Zenker <erikzenker@hotmail.com>
Rewritten by Rafael Sadowski <rafael@sizeofvoid.org>
