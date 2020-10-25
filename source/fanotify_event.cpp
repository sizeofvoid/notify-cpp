/*
 * Copyright (c) 2017 Erik Zenker <erikzenker@hotmail.com>
 * Copyright (c) 2018 Rafael Sadowski <rafael@sizeofvoid.org>
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

#include <notify-cpp/fanotify_event.h>

#include <sys/fanotify.h>

#include <iostream>

#include <cassert>

namespace notifycpp {
FanotifyEventHandler::FanotifyEventHandler(const Event e)
    : EventHandler(e)
{
}

std::uint32_t
FanotifyEventHandler::convertToEvents(const Event event) const
{
    return convert(event, std::bind(&FanotifyEventHandler::getEvent, this, std::placeholders::_1));
}

std::uint32_t
FanotifyEventHandler::getEvent(const Event e) const
{
    switch (e) {
    case Event::access:
        return FAN_ACCESS;
    case Event::modify:
        return FAN_MODIFY;
    case Event::attrib:
        assert(!"None existing event");
        return 0;
    case Event::close_write:
        return FAN_CLOSE_WRITE;
    case Event::close_nowrite:
        return FAN_CLOSE_NOWRITE;
    case Event::open:
        return FAN_OPEN;

    case Event::moved_from:
    case Event::moved_to:
    case Event::create:
    case Event::delete_sub:
    case Event::delete_self:
    case Event::move_self:
        assert(!"None existing event");
        return 0;

    case Event::close:
        return FAN_CLOSE;

    case Event::move:
    case Event::all:
    case Event::none:
        assert(!"None existing event");
        return 0;
    }
    assert(!"None existing event");
    return 0;
}

std::string FanotifyEventHandler::toString(std::uint32_t e) const
{
    const auto getString = [&](std::uint32_t myEvent) -> std::string {
        switch (myEvent) {
        case FAN_ACCESS:
            return std::string("access");
        case FAN_MODIFY:
            return std::string("modify");
        case FAN_CLOSE_WRITE:
            return std::string("close_write");
        case FAN_CLOSE_NOWRITE:
            return std::string("close_nowrite");
        case FAN_OPEN:
            return std::string("open");
        case FAN_Q_OVERFLOW:
            return std::string("overflow");
        case FAN_OPEN_PERM:
            return std::string("open_perm");
        case FAN_ONDIR:
            return std::string("ondir");
        case FAN_EVENT_ON_CHILD:
            return std::string("on_child");
        case FAN_CLOSE:
            return std::string("close");
        case FAN_ALL_CLASS_BITS:
            return std::string("all_class_bits");
        }
        return "NONE";
    };

    std::string events;
    for (auto const i : AllFanFlags) {
        if ((e & i) == i) {
            if (events.empty()) {
                events = getString(i);
            }
            else {
                events = events + ", " + getString(i);
            }
        }
    }
    return events;
}

std::vector<Event> FanotifyEventHandler::getEvents(std::uint32_t e) const
{
    std::vector<Event> events;
    for (auto const event : AllFanFlags) {
        std::uint32_t ie = static_cast<std::uint32_t>(event);
        if ((e & ie) == ie) {
            events.push_back(get(ie));
        }
    }
    return events;
}

Event FanotifyEventHandler::get(std::uint32_t e) const
{
    switch (e) {
    case FAN_ACCESS:
        return Event::access;
    case FAN_MODIFY:
        return Event::modify;
    case FAN_CLOSE_WRITE:
        return Event::close_write;
    case FAN_CLOSE_NOWRITE:
        return Event::close_nowrite;
    case FAN_OPEN:
        return Event::open;
    case FAN_CLOSE:
        return Event::close;
        /* TODO
        case FAN_Q_OVERFLOW:
        case FAN_OPEN_PERM:
        case FAN_ONDIR:
        case FAN_EVENT_ON_CHILD:
        case FAN_ALL_CLASS_BITS:
        */
    }
    return Event::none;
}

std::uint32_t
FanotifyEventHandler::convert(const Event event, std::function<std::uint32_t(Event)> translator) const
{
    std::uint32_t events = 0;
    for (const auto& e : AllEvents) {
        if ((event & e) == e) {
            events = events | translator(e);
        }
    }
    return events;
}
}
