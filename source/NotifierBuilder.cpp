#include <inotify-cpp/NotifierBuilder.h>

namespace inotify {

NotifierBuilder::NotifierBuilder()
    : _Fnotify(new Inotify())
{
}

NotifierBuilder BuildNotifier()
{
    return {};
}

auto NotifierBuilder::watchMountPoint(std::string path) -> NotifierBuilder&
{
    _Fnotify->watchMountPoint(path);
    return *this;
}

auto NotifierBuilder::watchFile(std::string file) -> NotifierBuilder&
{
    _Fnotify->watchFile(file);
    return *this;
}

auto NotifierBuilder::unwatch(std::string file) -> NotifierBuilder&
{
    _Fnotify->unwatch(file);
    return *this;
}

auto NotifierBuilder::ignoreFile(std::string file) -> NotifierBuilder&
{
    _Fnotify->ignoreFile(file);
    return *this;
}

auto NotifierBuilder::onEvent(Event event, EventObserver eventObserver) -> NotifierBuilder&
{
    _Fnotify->setEventMask(_Fnotify->getEventMask() | static_cast<std::uint64_t>(event));
    mEventObserver[event] = eventObserver;
    return *this;
}

auto NotifierBuilder::onEvents(std::vector<Event> events, EventObserver eventObserver)
    -> NotifierBuilder&
{
    for (auto event : events) {
        _Fnotify->setEventMask(_Fnotify->getEventMask() | static_cast<std::uint64_t>(event));
        mEventObserver[event] = eventObserver;
    }

    return *this;
}

auto NotifierBuilder::onUnexpectedEvent(EventObserver eventObserver) -> NotifierBuilder&
{
    mUnexpectedEventObserver = eventObserver;
    return *this;
}

auto NotifierBuilder::runOnce() -> void
{
    auto fileSystemEvent = _Fnotify->getNextEvent();
    if (!fileSystemEvent) {
        return;
    }
    Event event = static_cast<Event>(fileSystemEvent->mask);

    const auto eventAndEventObserver = mEventObserver.find(event);

    if (eventAndEventObserver == mEventObserver.end()) {
        if (mUnexpectedEventObserver) {
            mUnexpectedEventObserver({ event, fileSystemEvent->path });
        }
    } else {
        /* handle observed processes */
        auto eventObserver = eventAndEventObserver->second;
        eventObserver({ event, fileSystemEvent->path });
    }
}

auto NotifierBuilder::run() -> void
{
    while (true) {
        if (_Fnotify->hasStopped()) {
            break;
        }

        runOnce();
    }
}

auto NotifierBuilder::stop() -> void
{
    _Fnotify->stop();
}
}
