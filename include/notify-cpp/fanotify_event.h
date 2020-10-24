#pragma once

#include <notify-cpp/event.h>

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <linux/version.h>
#include <sys/fanotify.h>
#include <type_traits>
#include <vector>

namespace notifycpp {

// TODO Check with assert
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
// No Aduit support https://github.com/torvalds/linux/commit/de8cd83e91bc3ee212b3e6ec6e4283af9e4ab269
static const std::array<std::uint32_t, 12> AllFanFlags = {{FAN_ACCESS,
    FAN_MODIFY,
    FAN_CLOSE_WRITE,
    FAN_CLOSE_NOWRITE,
    FAN_OPEN,
    FAN_Q_OVERFLOW,
    FAN_OPEN_PERM,
    FAN_ONDIR,
    FAN_EVENT_ON_CHILD,
    FAN_CLOSE,
    FAN_ALL_CLASS_BITS}};
#else
static const std::array<std::uint32_t, 12> AllFanFlags = {{FAN_ACCESS,
    FAN_MODIFY,
    FAN_CLOSE_WRITE,
    FAN_CLOSE_NOWRITE,
    FAN_OPEN,
    FAN_Q_OVERFLOW,
    FAN_OPEN_PERM,
    FAN_ONDIR,
    FAN_EVENT_ON_CHILD,
    FAN_CLOSE,
    FAN_ALL_CLASS_BITS,
    FAN_ENABLE_AUDIT}};
#endif

class FanotifyEventHandler : EventHandler {
public:
    FanotifyEventHandler(const Event);
    FanotifyEventHandler() = default;

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
