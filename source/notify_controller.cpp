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

#include <notify-cpp/fanotify.h>
#include <notify-cpp/inotify.h>
#include <notify-cpp/notifier_builder.h>

namespace notifycpp {

FanotifyNotifierBuilder::FanotifyNotifierBuilder()
    : NotifierBuilder(new Fanotify)
{
}
NotifierBuilder& FanotifyNotifierBuilder::watchMountPoint(const std::filesystem::path& p)
{
    static_cast<Fanotify*>(_Notify)->watchMountPoint(p);
    return *this;
}

InotifyNotifierBuilder::InotifyNotifierBuilder()
    : NotifierBuilder(new Inotify)
{
}

NotifierBuilder::NotifierBuilder(Notify* n)
    : _Notify(n)
{
}

NotifierBuilder&
NotifierBuilder::watchFile(const FileSystemEvent& fse)
{
    _Notify->watchFile(fse);
    return *this;
}

NotifierBuilder&
NotifierBuilder::watchPathRecursively(const FileSystemEvent& fse)
{
    _Notify->watchPathRecursively(fse);
    return *this;
}

NotifierBuilder& NotifierBuilder::unwatch(const std::filesystem::path& f)
{
    _Notify->unwatch(f);
    return *this;
}

NotifierBuilder& NotifierBuilder::ignore(const std::filesystem::path& p)
{
    _Notify->ignore(p);
    return *this;
}
NotifierBuilder& NotifierBuilder::onEvent(Event event, EventObserver eventObserver)
{
    mEventObserver[event] = eventObserver;
    return *this;
}

NotifierBuilder& NotifierBuilder::onEvents(std::set<Event> events, EventObserver eventObserver)
{
    for (auto event : events)
        mEventObserver[event] = eventObserver;
    return *this;
}

NotifierBuilder&
NotifierBuilder::onUnexpectedEvent(EventObserver eventObserver)
{
    mUnexpectedEventObserver = eventObserver;
    return *this;
}

void NotifierBuilder::runOnce()
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

void NotifierBuilder::run()
{
    while (!_Notify->hasStopped())
        runOnce();
}

void NotifierBuilder::stop()
{
    _Notify->stop();
}

std::vector<std::pair<Event, EventObserver>>
NotifierBuilder::findObserver(Event e) const
{
    std::vector<std::pair<Event, EventObserver>> observers;
    for (auto const& event2Observer : mEventObserver)
        if ((event2Observer.first & e) == e)
            observers.emplace_back(event2Observer.first, event2Observer.second);
    return observers;
}
}
