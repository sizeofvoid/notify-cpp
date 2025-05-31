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

#include <notify-cpp/event.h>

#include <sys/fanotify.h>
#include <sys/inotify.h>

#include <iostream>

#include <cassert>

namespace notifycpp {

std::uint32_t
EventHandler::convertToInotifyEvents(const Event event) const
{
    return convert(event, std::bind(&EventHandler::getInotifyEvent, this, std::placeholders::_1));
}

std::uint32_t
EventHandler::convertToFanotifyEvents(const Event event) const
{
    return convert(event, std::bind(&EventHandler::getFanotifyEvent, this, std::placeholders::_1));
}

std::uint32_t
EventHandler::getInotifyEvent(const Event e) const
{
    switch (e) {
    case Event::access:
        return IN_ACCESS;
    case Event::modify:
        return IN_MODIFY;
    case Event::attrib:
        return IN_ATTRIB;
    case Event::close_write:
        return IN_CLOSE_WRITE;
    case Event::close_nowrite:
        return IN_CLOSE_NOWRITE;
    case Event::open:
        return IN_OPEN;
    case Event::moved_from:
        return IN_MOVED_FROM;
    case Event::moved_to:
        return IN_MOVED_TO;
    case Event::create:
        return IN_CREATE;
    case Event::delete_sub:
        return IN_DELETE;
    case Event::delete_self:
        return IN_DELETE_SELF;
    case Event::move_self:
        return IN_MOVE_SELF;
    case Event::close:
        return IN_CLOSE;
    case Event::move:
        return IN_MOVE;
    case Event::all:
        return IN_ALL_EVENTS;
    case Event::none:
        return 0;
    }
    return 0;
}

std::uint32_t
EventHandler::getFanotifyEvent(const Event e) const
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

std::string
toString(const Event event)
{
    const auto getString = [](Event myEvent) -> std::string {
        switch (myEvent) {
        case Event::access:
            return std::string("access");
        case Event::modify:
            return std::string("modify");
        case Event::attrib:
            return std::string("attrib");
        case Event::close_write:
            return std::string("close_write");
        case Event::close_nowrite:
            return std::string("close_nowrite");
        case Event::open:
            return std::string("open");
        case Event::moved_from:
            return std::string("moved_from");
        case Event::moved_to:
            return std::string("moved_to");
        case Event::create:
            return std::string("create");
        case Event::delete_sub:
            return std::string("delete");
        case Event::delete_self:
            return std::string("delete_self");
        case Event::move_self:
            return std::string("move_self");
        case Event::close:
            return std::string("close");
        case Event::move:
            return std::string("move");
        case Event::all:
            return std::string("all");
        case Event::none:
            return std::string("none");
        }
        assert(!"None existing event");
        return std::string("ERROR");
    };

    std::string events;
    for (const auto& e : AllEvents) {
        if ((event & e) == e) {
            if (events.empty()) {
                events = getString(e);
            }
            else {
                events = events + "," + getString(e);
            }
        }
    }
    return events;
}

std::ostream& operator<<(std::ostream& stream, const Event& event)
{
    stream << toString(event);
    return stream;
}

Event EventHandler::getInotify(std::uint32_t e) const
{
    switch (e) {
    case IN_ACCESS:
        return Event::access;
    case IN_MODIFY:
        return Event::modify;
    case IN_ATTRIB:
        return Event::attrib;
    case IN_CLOSE_WRITE:
        return Event::close_write;
    case IN_CLOSE_NOWRITE:
        return Event::close_nowrite;
    case IN_OPEN:
        return Event::open;
    case IN_MOVED_FROM:
        return Event::moved_from;
    case IN_MOVED_TO:
        return Event::moved_to;
    case IN_CREATE:
        return Event::create;
    case IN_DELETE:
        return Event::delete_sub;
    case IN_DELETE_SELF:
        return Event::delete_self;
    case IN_MOVE_SELF:
        return Event::move_self;
    case IN_CLOSE:
        return Event::close;
    case IN_MOVE:
        return Event::move;
    case IN_ALL_EVENTS:
        return Event::all;
    }
    return Event::none;
}

std::string EventHandler::getFanotifyStr(std::uint32_t e) const
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
    for (auto const i: AllFanFlags) {
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

std::vector<Event> EventHandler::getFanotifyEvents(std::uint32_t e) const
{
    std::vector<Event> events;
    for (auto const event : AllFanFlags) {
        std::uint32_t ie = static_cast<std::uint32_t>(event);
        if ((e & ie) == ie) {
            events.push_back(getFanotify(ie));
        }
    }
    return events;
}

Event EventHandler::getFanotify(std::uint32_t e) const
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
EventHandler::convert(const Event event, std::function<std::uint32_t(Event)> translator) const
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
