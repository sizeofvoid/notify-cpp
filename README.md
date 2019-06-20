# notify-cpp - A C++17 interface for linux monitoring filesystem events
[![Build Status](https://travis-ci.org/sizeofvoid/notify-cpp.svg?branch=master)](https://travis-ci.org/sizeofvoid/notify-cpp)
===========

__notify-cpp__ is a C++17 wrapper for linux fanotify and inotify. It lets you
watch for filesystem events on your filesystem tree. It's based on the work of
__erikzenker/inotify-cpp__.

## Build Library ##
```bash
mkdir build; cd bulid
cmake -DCMAKE_INSTALL_PREFIX=/opt/ ..
make

# install the library
make install
```

## Dependencies ##
 + C++ 17 Compiler
 + linux 2.6.13

## Licence
MIT

## Author ##
Initially written by Erik Zenker <erikzenker@hotmail.com>
Rewritten by Rafael Sadowski <rafael@sizeofvoid.org>
