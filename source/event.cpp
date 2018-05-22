/*
 * Copyright (c) 2017 Erik Zenker <erikzenker@hotmail.com>
 * Copyright (c) 2018 Rafael Sadowski <rafael.sadowski@computacenter.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <notify-cpp/event.h>

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace notifycpp {
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
std::string toString(const Event& event)
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
    if (containsEvent(event, Event::event_on_child))
        maskString.append("event_on_child ");
    if (containsEvent(event, Event::all_events))
        maskString.append("all_events ");
    if (containsEvent(event, Event::all_outgoing_events))
        maskString.append("all_outgoing_events ");

    return maskString;
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
    if (containsEvent(event, Event::event_on_child))
        maskString.append("event_on_child ");
    if (containsEvent(event, Event::all_events))
        maskString.append("all_events ");
    if (containsEvent(event, Event::all_outgoing_events))
        maskString.append("all_outgoing_events ");

    stream << maskString;
    return stream;
}
}
