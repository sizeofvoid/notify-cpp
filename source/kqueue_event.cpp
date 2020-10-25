/*
 * Copyright (c) 2020 Rafael Sadowski <rafael@sizeofvoid.org>
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

#include <notify-cpp/kqueue_event.h>


#include <sys/param.h>
#include <sys/types.h>
#include <sys/event.h>

#include <cassert>
#include <string>

namespace notifycpp {
KqueueEventHandler::KqueueEventHandler(const Event e)
    : EventHandler(e)
{
}

std::uint32_t
KqueueEventHandler::convertToEvents(const Event event) const
{
    return {};
}

std::uint32_t
KqueueEventHandler::getEvent(const Event e) const
{
    switch (e) {
    case Event::access:
    case Event::modify:
    case Event::attrib:
        return NOTE_WRITE;
    case Event::close_nowrite:
    case Event::open:
        return 0;
    case Event::moved_from:
    case Event::moved_to:
    case Event::create:
        return 0;
    case Event::delete_sub:
    case Event::delete_self:
        return NOTE_DELETE;
    case Event::move_self:
        return 0;
    case Event::close:
        return NOTE_ATTRIB;
    case Event::move:
    case Event::all:
    case Event::none:
        return 0;
    }
    return 0;
}

Event KqueueEventHandler::get(std::uint32_t e) const
{
    switch (e) {
    case NOTE_WRITE:
        return Event::none;
    }
    return Event::none;
}
}
