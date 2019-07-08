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

class NotifyController {
public:
    NotifyController(Notify*);
    NotifyController() = default;

    void run();

    void runOnce();

    void stop();

    NotifyController& watchFile(const FileSystemEvent&);

    NotifyController& watchPathRecursively(const FileSystemEvent&);

    NotifyController& unwatch(const std::filesystem::path&);

    NotifyController& ignore(const std::filesystem::path&);

    NotifyController& ignoreOnce(const std::filesystem::path&);

    NotifyController& onEvent(Event event, EventObserver);

    NotifyController& onEvents(std::set<Event> event, EventObserver);

    NotifyController& onUnexpectedEvent(EventObserver);

protected:
    Notify* _Notify;
    //std::unique_ptr<Notify> _Notify;

private:
    std::vector<std::pair<Event, EventObserver>> findObserver(Event e) const;

    std::map<Event, EventObserver> mEventObserver;

    EventObserver mUnexpectedEventObserver;
};

class FanotifyController : public NotifyController {
public:
    FanotifyController();

    NotifyController& watchMountPoint(const std::filesystem::path&);
};

class InotifyController : public NotifyController {
public:
    InotifyController();
};
}
