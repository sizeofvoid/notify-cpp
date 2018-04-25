#include <inotify-cpp/Event.h>

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace inotify {
Event operator|(Event lhs, Event rhs)
{
    return static_cast<Event>(
        static_cast<std::underlying_type<Event>::type>(lhs)
        | static_cast<std::underlying_type<Event>::type>(rhs));
}

Event operator&(Event lhs, Event rhs)
{
    return static_cast<Event>(
        static_cast<std::underlying_type<Event>::type>(lhs)
        & static_cast<std::underlying_type<Event>::type>(rhs));
}

bool containsEvent(const Event& allEvents, const Event& event)
{
    if (static_cast<std::uint64_t>(event & allEvents)) {
        return true;
    } else {
        return false;
    }
}

std::ostream& operator<<(std::ostream& stream, const Event& event)
{
    std::string maskString = "";

    if (containsEvent(event, Event::access))
        maskString.append("access ");
    if (containsEvent(event, Event::modify))
        maskString.append("modify ");
    if (containsEvent(event, Event::close_write))
        maskString.append("close_write ");
    if (containsEvent(event, Event::close_nowrite))
        maskString.append("close_nowrite ");
    if (containsEvent(event, Event::open))
        maskString.append("open ");
    if (containsEvent(event, Event::q_overflow))
        maskString.append("q_overflow ");
    if (containsEvent(event, Event::perm_open))
        maskString.append("perm_open ");
    if (containsEvent(event, Event::perm_access))
        maskString.append("perm_access ");
    if (containsEvent(event, Event::ondir))
        maskString.append("ondir ");
    if (containsEvent(event, Event::close))
        maskString.append("close ");
    if (containsEvent(event, Event::even_no_child))
        maskString.append("even_no_child ");
    if (containsEvent(event, Event::all_events))
        maskString.append("all_events ");
    if (containsEvent(event, Event::all_outgoing_events))
        maskString.append("all_outgoing_events ");

    stream << maskString;
    return stream;
}
}
