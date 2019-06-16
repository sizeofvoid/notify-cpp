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

#include <notify-cpp/fanotify.h>
#include <notify-cpp/inotify.h>
#include <notify-cpp/notify_controller.h>

namespace notifycpp {

FanotifyController::FanotifyController()
    : NotifyController(new Fanotify)
{
}

NotifyController& FanotifyController::watchMountPoint(const std::filesystem::path& p)
{
    static_cast<Fanotify*>(_Notify)->watchMountPoint(p);
    return *this;
}

InotifyController::InotifyController()
    : NotifyController(new Inotify)
{
}

NotifyController::NotifyController(Notify* n)
    : _Notify(n)
{
}

NotifyController&
NotifyController::watchFile(const FileSystemEvent& fse)
{
    _Notify->watchFile(fse);
    return *this;
}

NotifyController&
NotifyController::watchPathRecursively(const FileSystemEvent& fse)
{
    _Notify->watchPathRecursively(fse);
    return *this;
}

NotifyController& NotifyController::unwatch(const std::filesystem::path& f)
{
    _Notify->unwatch(f);
    return *this;
}

NotifyController& NotifyController::ignore(const std::filesystem::path& p)
{
    _Notify->ignore(p);
    return *this;
}

NotifyController& NotifyController::onEvent(Event event, EventObserver eventObserver)
{
    mEventObserver[event] = eventObserver;
    return *this;
}

NotifyController& NotifyController::onEvents(std::set<Event> events, EventObserver eventObserver)
{
    for (auto event : events)
        mEventObserver[event] = eventObserver;
    return *this;
}

NotifyController&
NotifyController::onUnexpectedEvent(EventObserver eventObserver)
{
    mUnexpectedEventObserver = eventObserver;
    return *this;
}

void NotifyController::runOnce()
{
    auto fileSystemEvent = _Notify->getNextEvent();
    if (!fileSystemEvent) {
        return;
    }

    const Event event = fileSystemEvent->getEvent();
    const auto observers = findObserver(event);

    if (observers.empty()) {
        if (mUnexpectedEventObserver) {
            mUnexpectedEventObserver({event, fileSystemEvent->getPath()});
        }
    }
    else {
        for (const auto& observerEvent : observers) {
            /* handle observed processes */
            auto eventObserver = observerEvent.second;
            eventObserver({observerEvent.first, fileSystemEvent->getPath()});
        }
    }
}

void NotifyController::run()
{
    while (!_Notify->hasStopped())
        runOnce();
}

void NotifyController::stop()
{
    _Notify->stop();
}

std::vector<std::pair<Event, EventObserver>>
NotifyController::findObserver(Event e) const
{
    std::vector<std::pair<Event, EventObserver>> observers;
    for (auto const& event2Observer : mEventObserver)
        if ((event2Observer.first & e) == e)
            observers.emplace_back(event2Observer.first, event2Observer.second);
    return observers;
}
}
