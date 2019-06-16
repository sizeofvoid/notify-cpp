#pragma once

#include <notify-cpp/notification.h>
#include <notify-cpp/notify.h>

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>

namespace notifycpp {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
public:
    NotifierBuilder(Notify*);
    NotifierBuilder() = default;

    void run();

    void runOnce();

    void stop();

    NotifierBuilder& watchFile(const FileSystemEvent&);

    NotifierBuilder& watchPathRecursively(const FileSystemEvent&);

    NotifierBuilder& unwatch(const std::filesystem::path&);

    NotifierBuilder& ignore(const std::filesystem::path&);

    NotifierBuilder& onEvent(Event event, EventObserver);

    NotifierBuilder& onEvents(std::set<Event> event, EventObserver);

    NotifierBuilder& onUnexpectedEvent(EventObserver);

protected:
    Notify* _Notify;
    //std::unique_ptr<Notify> _Notify;

private:
    std::vector<std::pair<Event, EventObserver>> findObserver(Event e) const;

    std::map<Event, EventObserver> mEventObserver;

    EventObserver mUnexpectedEventObserver;
};

class FanotifyNotifierBuilder : public NotifierBuilder {
public:
    FanotifyNotifierBuilder();

    NotifierBuilder& watchMountPoint(const std::filesystem::path&);
};

class InotifyNotifierBuilder : public NotifierBuilder {
public:
    InotifyNotifierBuilder();
};
}
