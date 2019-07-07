# notify-cpp - A C++17 interface for linux monitoring filesystem events
[![Build Status](https://travis-ci.org/sizeofvoid/notify-cpp.svg?branch=master)](https://travis-ci.org/sizeofvoid/notify-cpp)
===========

__notify-cpp__ is a C++17 wrapper for linux fanotify and inotify. It lets you
watch for filesystem events on your filesystem tree. It's based on the work of
__erikzenker/inotify-cpp__.


## How to use notif-cpp

### A simple file monitor with a [inotify(7)](http://man7.org/linux/man-pages/man7/inotify.7.html) or [fanotify(7)](http://man7.org/linux/man-pages/man7/fanotify.7.html) backend.
```cpp
#include <notify-cpp/notify_controller.h>

#include <filesystem>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    const auto usage = [](){
        std::cout << "Usage: ./file_monitor fanotify|inotify /path/to/file" << std::endl;
        exit(0);
    };

    // Create the linux notification backend
    const auto createBackend = [&usage](const std::string& backend) -> notifycpp::NotifyController {
        if (backend != std::string("fanotify") || backend != std::string("inotify")) {
            usage();
        }
        if (backend == std::string("inotify"))
            return notifycpp::InotifyController();
        return notifycpp::FanotifyController();
    };

    if (argc <= 2) {
        usage();
    }

    const std::filesystem::path towatch(argv[2]);

    const notifycpp::Event watchOn = notifycpp::Event::open
                                     | notifycpp::Event::close_write;


    // Set the ::getEvent() handler which will be used to process particular events
    auto handleNotification = [&](const notifycpp::Notification& notify) {
        std::cout << "log: event " << notify.getEvent() << " on " << notify.getPath() << std::endl;
    };

    notifycpp::NotifyController notifier = createBackend(argv[1]);

    notifier.watchFile({towatch, watchOn}).onEvents({notifycpp::Event::open,
                                                     notifycpp::Event::close_write},
                                                     handleNotification);

    // The getEvent() loop is started in a separate thread context.
    std::thread thread([&]() { notifier.run(); });

    // Terminate the getEvent() loop after 60 seconds
    std::this_thread::sleep_for(std::chrono::seconds(60));
    notifier.stop();
    thread.join();
    return 0;
}
```

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
