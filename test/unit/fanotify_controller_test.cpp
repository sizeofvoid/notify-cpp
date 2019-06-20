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
#include <notify-cpp/inotify.h>
#include <notify-cpp/notify_controller.h>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>

using namespace notifycpp;

void openFile(std::filesystem::path file)
{
    std::ofstream stream;
    stream.open(file.string(), std::ifstream::out);
    BOOST_CHECK(stream.is_open());
    stream << "Writing this to a file.\n";
    stream.close();
}

struct FanotifyControllerTest {
    FanotifyControllerTest()
        : testDirectory_("testDirectory")
        , recursiveTestDirectory_(testDirectory_ / "recursiveTestDirectory")
        , testFileOne_(testDirectory_ / "test.txt")
        , testFileTwo_(testDirectory_ / "test2.txt")
        , timeout_(1)
    {
        std::filesystem::create_directories(testDirectory_);
        std::ofstream streamOne(testFileOne_);
        std::ofstream streamTwo(testFileTwo_);
    }

    ~FanotifyControllerTest() = default;

    std::filesystem::path testDirectory_;
    std::filesystem::path recursiveTestDirectory_;
    std::filesystem::path testFileOne_;
    std::filesystem::path testFileTwo_;

    std::chrono::seconds timeout_;

    // Events
    std::promise<Notification> promisedOpen_;
    std::promise<Notification> promisedCloseNoWrite_;
};

BOOST_AUTO_TEST_CASE(EventOperatorTest)
{
    BOOST_CHECK((Event::all & Event::close_write) == Event::close_write);
    BOOST_CHECK((Event::all & Event::moved_from) == Event::moved_from);
    BOOST_CHECK((Event::move & Event::moved_from) == Event::moved_from);
    BOOST_CHECK(!((Event::move & Event::open) == Event::open));
    BOOST_CHECK(toString(Event::access) == std::string("access"));
}

BOOST_FIXTURE_TEST_CASE(shouldNotAcceptNotExistingPaths, FanotifyControllerTest)
{
    BOOST_CHECK_THROW(FanotifyController().watchPathRecursively(std::filesystem::path("/not/existing/path/")), std::invalid_argument);
    BOOST_CHECK_THROW(FanotifyController().watchFile(std::filesystem::path("/not/existing/file")), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnOpenEvent, FanotifyControllerTest)
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
/*
BOOST_FIXTURE_TEST_CASE(shouldNotifyOnMultipleEvents, FanotifyControllerTest)
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

BOOST_FIXTURE_TEST_CASE(shouldStopRunOnce, FanotifyControllerTest)
{
    NotifyController notifier = FanotifyController().watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldStopRun, FanotifyControllerTest)
{
    FanotifyController notifier = FanotifyController();
    notifier.watchFile(testFileOne_);

    std::thread thread([&notifier]() { notifier.run(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFile, FanotifyControllerTest)
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

BOOST_FIXTURE_TEST_CASE(shouldUnwatchPath, FanotifyControllerTest)
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

BOOST_FIXTURE_TEST_CASE(shouldCallUserDefinedUnexpectedExceptionObserver, FanotifyControllerTest)
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
*/
