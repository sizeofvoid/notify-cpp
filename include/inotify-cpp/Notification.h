#pragma once

#include <inotify-cpp/Event.h>

namespace inotify {

class Notification {
  public:
    Notification(Event, const std::string&);

    std::string getPath() const;
    Event getEvent() const;

  private:
    Event _Event;
    std::string _Path;
};
}
