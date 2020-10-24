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

#include <notify-cpp/event.h>

#include <string>
#include <cassert>

namespace notifycpp {
EventHandler::EventHandler(const Event e)
    : _Events(e)
{
}

std::string
EventHandler::toString(const Event event)
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
}
