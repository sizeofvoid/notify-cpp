#pragma once

#include <inotify-cpp/Inotify.h>
#include <inotify-cpp/Notification.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace inotify {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder();

    auto run() -> void;
    auto runOnce() -> void;
    auto stop() -> void;
    auto watchMountPoint(std::string path) -> NotifierBuilder&;
    auto watchFile(std::string file) -> NotifierBuilder&;
    auto unwatch(std::string file) -> NotifierBuilder&;
    auto ignoreFile(std::string file) -> NotifierBuilder&;
    auto onEvent(Event event, EventObserver) -> NotifierBuilder&;
    auto onEvents(std::vector<Event> event, EventObserver) -> NotifierBuilder&;
    auto onUnexpectedEvent(EventObserver) -> NotifierBuilder&;

  private:
    std::unique_ptr<Inotify> _Fnotify;
    std::map<Event, EventObserver> mEventObserver;
    EventObserver mUnexpectedEventObserver;
};

NotifierBuilder BuildNotifier();
}
