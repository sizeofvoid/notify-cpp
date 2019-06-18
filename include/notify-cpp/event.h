#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <type_traits>

namespace notifycpp {
template <typename Enum>
struct EnableBitMaskOperators {
    static const bool enable = false;
};

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator|(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator&(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

enum class Event {
    access = (1 << 0),
    modify = (1 << 1),
    attrib = (1 << 2),
    close_write = (1 << 3),
    close_nowrite = (1 << 4),
    open = (1 << 5),
    moved_from = (1 << 6),
    moved_to = (1 << 7),
    create = (1 << 8),
    delete_sub = (1 << 9),
    delete_self = (1 << 10),
    move_self = (1 << 11),

    // helper
    close = Event::close_write | Event::close_nowrite,

    move = Event::moved_from | Event::moved_to,

    all = Event::access | Event::modify | Event::attrib | Event::close_write
        | Event::close_nowrite | Event::open | Event::moved_from | Event::moved_to
        | Event::create | Event::delete_sub | Event::delete_self | Event::move_self
};
// TODO Check with assert
const std::array<Event, 15> AllEvents = {Event::access, Event::modify, Event::attrib, Event::close_write, Event::close_nowrite, Event::open, Event::moved_from, Event::moved_to, Event::create, Event::delete_sub, Event::delete_self, Event::move_self, Event::close, Event::move, Event::all};

template <>
struct EnableBitMaskOperators<Event> {
    static const bool enable = true;
};

class EventHandler {
public:
    EventHandler(const Event);
    EventHandler() = default;

    std::uint32_t convertToInotifyEvents(const Event) const;
    std::uint32_t convertToFanotifyEvents(const Event) const;
    std::uint32_t getInotifyEvent(const Event) const;
    std::uint32_t getFanotifyEvent(const Event) const;

    Event getInotify(std::uint32_t) const;
    Event getFanotify(std::uint32_t) const;

private:
    std::uint32_t convert(const Event, std::function<std::uint32_t(Event)>) const;
    const Event _Events = Event::all;
};

std::string toString(const Event);
std::ostream& operator<<(std::ostream&, const Event&);
}
