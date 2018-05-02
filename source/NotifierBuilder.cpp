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

#include <inotify-cpp/NotifierBuilder.h>
#include <inotify-cpp/fanotify.h>

namespace inotify {

FanotifyNotifierBuilder::FanotifyNotifierBuilder()
    : NotifierBuilder(new Fanotify)
{
}

NotifierBuilder::NotifierBuilder(Notify* n)
    : _Notify(n)
{
}

auto NotifierBuilder::watchMountPoint(std::string path) -> NotifierBuilder&
{
    _Notify->watchMountPoint(path);
    return *this;
}

auto NotifierBuilder::watchFile(std::string file) -> NotifierBuilder&
{
    _Notify->watchFile(file);
    return *this;
}

auto NotifierBuilder::unwatch(std::string file) -> NotifierBuilder&
{
    _Notify->unwatch(file);
    return *this;
}

auto NotifierBuilder::ignoreFile(std::string file) -> NotifierBuilder&
{
    _Notify->ignoreFile(file);
    return *this;
}

auto NotifierBuilder::onEvent(Event event, EventObserver eventObserver) -> NotifierBuilder&
{
    _Notify->setEventMask(_Notify->getEventMask() | static_cast<std::uint64_t>(event));
    mEventObserver[event] = eventObserver;
    return *this;
}

auto NotifierBuilder::onEvents(std::vector<Event> events, EventObserver eventObserver)
    -> NotifierBuilder&
{
    for (auto event : events) {
        _Notify->setEventMask(_Notify->getEventMask() | static_cast<std::uint64_t>(event));
        mEventObserver[event] = eventObserver;
    }

    return *this;
}

auto NotifierBuilder::onUnexpectedEvent(EventObserver eventObserver) -> NotifierBuilder&
{
    mUnexpectedEventObserver = eventObserver;
    return *this;
}

auto NotifierBuilder::runOnce() -> void
{
    auto fileSystemEvent = _Notify->getNextEvent();
    if (!fileSystemEvent) {
        return;
    }
    Event event = static_cast<Event>(fileSystemEvent->getMask());

    const auto eventAndEventObserver = mEventObserver.find(event);

    if (eventAndEventObserver == mEventObserver.end()) {
        if (mUnexpectedEventObserver) {
            mUnexpectedEventObserver({ event, fileSystemEvent->getPath() });
        }
    } else {
        /* handle observed processes */
        auto eventObserver = eventAndEventObserver->second;
        eventObserver({ event, fileSystemEvent->getPath() });
    }
}

auto NotifierBuilder::run() -> void
{
    while (!_Notify->hasStopped())
        runOnce();
}

auto NotifierBuilder::stop() -> void
{
    _Notify->stop();
}
}
