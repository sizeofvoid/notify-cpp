/*
 * Copyright (c) 2019 Rafael Sadowski <rafael@sizeovoid.org>
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
#include <notify-cpp/fanotify_controller.h>
#include <notify-cpp/fanotify_event.h>

#include "filesystem_event_helper.hpp"

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>

using namespace notifycpp;


BOOST_FIXTURE_TEST_CASE(shouldNotifyOnMultipleEvents, FilesystemEventHelper)
{
    FanotifyController notifier = FanotifyController();

    Event watchOn = Event::open | Event::close_write;

    notifier.watchFile({testFileOne_, watchOn}).onEvents({Event::open, Event::close_write}, [&](Notification notification) {
        switch (notification.getEvent()) {
        case Event::open:
            promisedOpen_.set_value(notification);
            break;
        case Event::close_write:
            promisedCloseNoWrite_.set_value(notification);
            break;
        default:
            break;
        }
    });

    std::thread thread([&notifier]() {
        notifier.runOnce();
        notifier.runOnce();
    });

    openFile(testFileOne_);

    auto futureOpen = promisedOpen_.get_future();
    auto futureCloseNoWrite = promisedCloseNoWrite_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpen.get().getEvent() == Event::open);
    BOOST_CHECK(futureCloseNoWrite.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureCloseNoWrite.get().getEvent() == Event::close_write);
    thread.join();
}

BOOST_AUTO_TEST_CASE(EventOperatorTest)
{
    BOOST_CHECK((Event::all & Event::close_write) == Event::close_write);
    BOOST_CHECK((Event::all & Event::moved_from) == Event::moved_from);
    BOOST_CHECK((Event::move & Event::moved_from) == Event::moved_from);
    BOOST_CHECK(!((Event::move & Event::open) == Event::open));
    BOOST_CHECK(toString(Event::access) == std::string("access"));
}

BOOST_FIXTURE_TEST_CASE(shouldNotAcceptNotExistingPaths, FilesystemEventHelper)
{
    BOOST_CHECK_THROW(FanotifyController().watchPathRecursively(std::filesystem::path("/not/existing/path/")), std::invalid_argument);
    BOOST_CHECK_THROW(FanotifyController().watchFile(std::filesystem::path("/not/existing/file")), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnOpenEvent, FilesystemEventHelper)
{
    NotifyController notifier = FanotifyController().watchFile({testFileOne_, Event::close}).onEvent(Event::close, [&](Notification notification) {
        promisedOpen_.set_value(notification);
    });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    auto futureOpenEvent = promisedOpen_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    const auto notify = futureOpenEvent.get();
    BOOST_CHECK_EQUAL(notify.getEvent(), Event::close);
    auto fullpath = std::filesystem::current_path();
    fullpath /= testFileOne_;
    BOOST_CHECK_EQUAL(notify.getPath(), fullpath);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldStopRunOnce, FilesystemEventHelper)
{
    NotifyController notifier = FanotifyController().watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldStopRun, FilesystemEventHelper)
{
    FanotifyController notifier = FanotifyController();
    notifier.watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.run(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFile, FilesystemEventHelper)
{
    NotifyController notifier = FanotifyController().ignore(testFileOne_).watchFile({testFileOne_, Event::close}).onEvent(Event::close, [&](Notification notification) {
        promisedOpen_.set_value(notification);
    });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    auto futureOpenEvent = promisedOpen_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::timeout);
    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldUnwatchPath, FilesystemEventHelper)
{
    std::promise<Notification> timeoutObserved;
    std::chrono::milliseconds timeout(100);

    FanotifyController notifier = FanotifyController();
    notifier.watchFile(testFileOne_).unwatch(testFileOne_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);
    BOOST_CHECK(promisedOpen_.get_future().wait_for(timeout_) != std::future_status::ready);
    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldCallUserDefinedUnexpectedExceptionObserver, FilesystemEventHelper)
{
    std::promise<void> observerCalled;

    NotifyController notifier2 = FanotifyController().watchFile(testFileOne_).onUnexpectedEvent([&](Notification) { observerCalled.set_value(); });

    NotifyController notifier = FanotifyController();
    notifier.watchFile(testFileOne_).onUnexpectedEvent([&](Notification) { observerCalled.set_value(); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    BOOST_CHECK(observerCalled.get_future().wait_for(timeout_) == std::future_status::ready);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldWatchPathRecursively, FilesystemEventHelper)
{
    FanotifyController notifier = FanotifyController();
    notifier.watchPathRecursively(testDirectory_)
            .onEvent(Event::open, [&](Notification notification) {
                switch (notification.getEvent()) {
                    case Event::open:
                        promisedOpen_.set_value(notification);
                        break;
                }

            });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFileOnce, FilesystemEventHelper)
{
    size_t counter = 0;
    FanotifyController notifier = FanotifyController();
    notifier.watchFile(testFileOne_).ignoreOnce(testFileOne_).onEvent(
            Event::open, [&](Notification notification) {
                ++counter;
                if (counter == 1)
                    _promisedCounter.set_value(counter);
            });

    std::thread thread([&notifier]() { notifier.run(); });

    // Known bug if the events to fast on the same file inotify(7) create only one event so we have to wait
    openFile(testFileOne_);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    openFile(testFileOne_);

    auto futureOpen = _promisedCounter.get_future();
    BOOST_CHECK(futureOpen.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
    notifier.stop();
    thread.join();
}
