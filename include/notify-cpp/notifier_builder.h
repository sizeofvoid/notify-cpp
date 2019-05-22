#pragma once

#include <notify-cpp/notification.h>
#include <notify-cpp/notify.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <filesystem>

namespace notifycpp {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder(Notify*);
    NotifierBuilder() = default;

    void run();

    void runOnce();

    void stop();

    NotifierBuilder& watchMountPoint(const std::filesystem::path&);

    NotifierBuilder& watchFile(const std::filesystem::path&);

    NotifierBuilder& unwatch(const std::filesystem::path&);

    NotifierBuilder& ignore(const std::filesystem::path&);

    NotifierBuilder& onEvent(Event event, EventObserver);

    NotifierBuilder& onEvents(std::vector<Event> event, EventObserver);

    NotifierBuilder& onUnexpectedEvent(EventObserver);

  protected:
    Notify* _Notify;
    //std::unique_ptr<Notify> _Notify;

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
