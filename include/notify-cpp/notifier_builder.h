#pragma once

#include <notify-cpp/notification.h>
#include <notify-cpp/notify.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace notifycpp {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder(Notify*);
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

  protected:
    std::unique_ptr<Notify> _Notify;

  private:
    std::map<Event, EventObserver> mEventObserver;
    EventObserver mUnexpectedEventObserver;
};

class FanotifyNotifierBuilder : public NotifierBuilder {
  public:
    FanotifyNotifierBuilder();
};

class InotifyNotifierBuilder : public NotifierBuilder {
  public:
    InotifyNotifierBuilder();
};
}
