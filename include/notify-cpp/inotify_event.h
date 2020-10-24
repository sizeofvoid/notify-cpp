#pragma once

#include <notify-cpp/event.h>

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#if defined(__linux__)
#include <linux/version.h>
#include <sys/fanotify.h>
#endif
#include <type_traits>
#include <vector>

namespace notifycpp {

class InotifyEventHandler : EventHandler {
public:
    InotifyEventHandler(const Event);
    InotifyEventHandler() = default;

    virtual std::uint32_t convertToEvents(const Event) const;
    virtual std::uint32_t getEvent(const Event) const;
    virtual std::vector<Event> getEvents(std::uint32_t) const;
    virtual Event get(std::uint32_t) const;
    virtual std::string toString(std::uint32_t) const;

private:
    std::uint32_t convert(const Event, std::function<std::uint32_t(Event)>) const;
    const Event _Events = Event::all;
};
}
