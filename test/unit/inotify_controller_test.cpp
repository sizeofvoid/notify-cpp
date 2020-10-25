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
#include <notify-cpp/inotify.h>

#include <notify-cpp/event.h>
#include <notify-cpp/notify_controller.h>

#include "doctest.h"

#include "filesystem_event_helper.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>

/*
 * The test cases based on the original work from Erik Zenker for inotify-cpp.
 * In to guarantee a compatibility with inotify-cpp the tests were mostly
 * unchanged.
 */
using namespace notifycpp;

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldNotAcceptNotExistingPaths")
{
    CHECK_THROWS_AS(InotifyController().watchPathRecursively(std::filesystem::path("/not/existing/path/")), std::invalid_argument);
    CHECK_THROWS_AS(InotifyController().watchFile(std::filesystem::path("/not/existing/file")), std::invalid_argument);
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldNotifyOnOpenEvent")
{
    NotifyController notifier = InotifyController().watchFile({testFileOne_, Event::close}).onEvent(Event::close, [&](Notification notification) {
        promisedOpen_.set_value(notification);
    });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    auto futureOpenEvent = promisedOpen_.get_future();
    CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    const auto notify = futureOpenEvent.get();
    CHECK(notify.getEvent() == Event::close);
    CHECK(notify.getPath() == testFileOne_);
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldNotifyOnMultipleEvents")
{
    InotifyController notifier = InotifyController();

    Event watchOn = Event::open | Event::close_write;
    CHECK((watchOn & Event::close_write) == Event::close_write);
    CHECK((watchOn & Event::open) == Event::open);
    CHECK((watchOn & Event::moved_from) != Event::moved_from);

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
    CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);
    CHECK(futureOpen.get().getEvent() == Event::open);
    CHECK(futureCloseNoWrite.wait_for(timeout_) == std::future_status::ready);
    CHECK(futureCloseNoWrite.get().getEvent() == Event::close_write);
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldStopRunOnce")
{
    NotifyController notifier = InotifyController().watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    notifier.stop();

    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldStopRun")
{
    InotifyController notifier = InotifyController();
    notifier.watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.run(); });

    notifier.stop();

    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldIgnoreFileOnce")
{
    size_t counter = 0;
    InotifyController notifier = InotifyController();
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
    CHECK(futureOpen.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
    notifier.stop();
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldIgnoreFile")
{
    NotifyController notifier = InotifyController().ignore(testFileOne_).watchFile({testFileOne_, Event::close}).onEvent(Event::close, [&](Notification notification) {
        promisedOpen_.set_value(notification);
    });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    auto futureOpenEvent = promisedOpen_.get_future();
    CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::timeout);
    notifier.stop();
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldWatchPathRecursively")
{
    InotifyController notifier = InotifyController();
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
    CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldUnwatchPath")
{
    std::promise<Notification> timeoutObserved;
    std::chrono::milliseconds timeout(100);

    InotifyController notifier = InotifyController();
    notifier.watchFile(testFileOne_).unwatch(testFileOne_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);
    CHECK(promisedOpen_.get_future().wait_for(timeout_) != std::future_status::ready);
    notifier.stop();
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "shouldCallUserDefinedUnexpectedExceptionObserver")
{
    std::promise<void> observerCalled;

    NotifyController notifier2 = InotifyController().watchFile(testFileOne_).onUnexpectedEvent([&](Notification) { observerCalled.set_value(); });

    NotifyController notifier = InotifyController();
    notifier.watchFile(testFileOne_).onUnexpectedEvent([&](Notification) { observerCalled.set_value(); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFileOne_);

    CHECK(observerCalled.get_future().wait_for(timeout_) == std::future_status::ready);
    thread.join();
}

TEST_CASE_FIXTURE(FilesystemEventHelper, "countEvents")
{
    size_t counter = 0;
    InotifyController notifier = InotifyController();
    notifier.watchFile(testFileOne_).onEvent(
            Event::open, [&](Notification notification) {
                ++counter;
                if (counter == 2)
                    _promisedCounter.set_value(counter);
            });

    std::thread thread([&notifier]() { notifier.run(); });

    // Known bug if the events to fast on the same file inotify(7) create only one event so we have to wait
    openFile(testFileOne_);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    openFile(testFileOne_);

    auto futureOpen = _promisedCounter.get_future();
    CHECK(futureOpen.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
    CHECK(futureOpen.get() == 2);
    notifier.stop();
    thread.join();
}
