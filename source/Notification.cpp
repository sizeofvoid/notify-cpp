#include <inotify-cpp/Notification.h>

namespace inotify {

Notification::Notification(Event event, const std::string& path)
    : _Event(event)
    , _Path(path)
{
}

std::string Notification::getPath() const
{
    return _Path;
}

Event Notification::getEvent() const
{
    return _Event;
}
}
