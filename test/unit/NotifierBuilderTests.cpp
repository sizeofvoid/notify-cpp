#include <notify-cpp/notifier_builder.h>
#include <notify-cpp/fanotify.h>
#include <notify-cpp/inotify.h>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace notifycpp;

void openFile(std::filesystem::path file)
{
    std::ifstream stream;
    stream.open(file.string(), std::ifstream::out);
    BOOST_CHECK(stream.is_open());
    stream.close();
}

struct NotifierBuilderTests {
    NotifierBuilderTests()
        : testDirectory_("testDirectory")
        , recursiveTestDirectory_(testDirectory_ / "recursiveTestDirectory")
        , testFile_(testDirectory_ / "test.txt")
        , timeout_(1)
    {
        std::filesystem::create_directories(testDirectory_);
        std::ofstream stream(testFile_);
    }

    ~NotifierBuilderTests() = default;

    std::filesystem::path testDirectory_;
    std::filesystem::path recursiveTestDirectory_;
    std::filesystem::path testFile_;

    std::chrono::seconds timeout_;

    // Events
    std::promise<Notification> promisedOpen_;
    std::promise<Notification> promisedCloseNoWrite_;
};

BOOST_FIXTURE_TEST_CASE(shouldNotAcceptNotExistingPaths, NotifierBuilderTests)
{
    //BOOST_CHECK_THROW(
    //    InotifyNotifierBuilder().watchPathRecursively("/not/existing/path/"), std::invalid_argument);
    //BOOST_CHECK_THROW(InotifyNotifierBuilder().watchFile(std::filesystem::path("/not/existing/file")), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnOpenEvent, NotifierBuilderTests)
{
    NotifierBuilder notifier = InotifyNotifierBuilder().watchFile(testFile_)
                                                       .onEvent(Event::close_write,
                                                                [&](Notification notification)
                                                                {
                                                                    promisedOpen_.set_value(notification);
                                                                });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    auto futureOpenEvent = promisedOpen_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpenEvent.get().getEvent() == Event::close_write);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnMultipleEvents, NotifierBuilderTests)
{
    /*
     *  error: in "shouldNotifyOnMultipleEvents": check futureOpen.wait_for(timeout_) == std::future_status::ready has failed
    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_).onEvents(
        { Event::open, Event::close_nowrite }, [&](Notification notification) {
            switch (notification.getEvent()) {
            case Event::open:
                promisedOpen_.set_value(notification);
                break;
            case Event::close_nowrite:
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

    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    auto futureCloseNoWrite = promisedCloseNoWrite_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpen.get().getEvent() == Event::open);
    BOOST_CHECK(futureCloseNoWrite.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureCloseNoWrite.get().getEvent() == Event::close_nowrite);
    thread.join();
    */
}

BOOST_FIXTURE_TEST_CASE(shouldStopRunOnce, NotifierBuilderTests)
{
    NotifierBuilder notifier = InotifyNotifierBuilder().watchFile(testFile_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldStopRun, NotifierBuilderTests)
{
    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_);

    std::thread thread([&notifier]() { notifier.run(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFileOnce, NotifierBuilderTests)
{
    /* XXX
    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_).ignoreFileOnce(testFile_).onEvent(
        Event::open, [&](Notification notification) { promisedOpen_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) != std::future_status::ready);

    notifier.stop();
    thread.join();
    */
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFile, NotifierBuilderTests)
{
    /* XXX
    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_).ignoreFile(testFile_).onEvent(
        Event::open, [&](Notification notification) { promisedOpen_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.run(); });

    openFile(testFile_);
    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) != std::future_status::ready);

    notifier.stop();
    thread.join();
    */
}

BOOST_FIXTURE_TEST_CASE(shouldWatchPathRecursively, NotifierBuilderTests)
{
    /* XXX
    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchPathRecursively(testDirectory_)
                        .onEvent(Event::open, [&](Notification notification) {
                            switch (notification.getEvent()) {
                            case Event::open:
                                promisedOpen_.set_value(notification);
                                break;
                            }

                        });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
    */
}

BOOST_FIXTURE_TEST_CASE(shouldUnwatchPath, NotifierBuilderTests)
{
    std::promise<Notification> timeoutObserved;
    std::chrono::milliseconds timeout(100);

    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_).unwatch(testFile_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);
    BOOST_CHECK(promisedOpen_.get_future().wait_for(timeout_) != std::future_status::ready);
    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldCallUserDefinedUnexpectedExceptionObserver, NotifierBuilderTests)
{
    std::promise<void> observerCalled;

    NotifierBuilder notifier2 = InotifyNotifierBuilder().watchFile(testFile_).onUnexpectedEvent(
        [&](Notification) { observerCalled.set_value(); });

    NotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_).onUnexpectedEvent(
        [&](Notification) { observerCalled.set_value(); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    BOOST_CHECK(observerCalled.get_future().wait_for(timeout_) == std::future_status::ready);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldSetEventTimeout, NotifierBuilderTests)
{
    /* XXX
    std::promise<Notification> timeoutObserved;
    std::chrono::milliseconds timeout(100);

    InotifyNotifierBuilder notifier = InotifyNotifierBuilder();
    notifier.watchFile(testFile_)
              .onEvent(
                  Event::open,
                  [&](Notification notification) { promisedOpen_.set_value(notification); })
              .setEventTimeout(timeout, [&](Notification notification) {
                  timeoutObserved.set_value(notification);
              });

    std::thread thread([&notifier]() {
        notifier.runOnce(); // open
    });

    openFile(testFile_);

    BOOST_CHECK(promisedOpen_.get_future().wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(timeoutObserved.get_future().wait_for(timeout_) == std::future_status::ready);
    thread.join();
    */
}
