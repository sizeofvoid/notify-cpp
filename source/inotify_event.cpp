/**
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

#include <notify-cpp/inotify_event.h>

#include <sys/inotify.h>

#include <iostream>

#include <cassert>

namespace notifycpp {
InotifyEventHandler::InotifyEventHandler(const Event e)
    : _Events(e)
{
}

std::uint32_t
InotifyEventHandler::convertToEvents(const Event event) const
{
    return convert(event, std::bind(&InotifyEventHandler::getEvent, this, std::placeholders::_1));
}

std::uint32_t
InotifyEventHandler::getEvent(const Event e) const
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

Event InotifyEventHandler::get(std::uint32_t e) const
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

std::uint32_t
InotifyEventHandler::convert(const Event event, std::function<std::uint32_t(Event)> translator) const
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
