#pragma once

#include <inotify-cpp/Event.h>

namespace inotify {

struct Notification {
    Event event;
    std::string path;
};
}
