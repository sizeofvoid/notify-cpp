#include <inotify-cpp/Inotify.h>

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <functional>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

namespace inotify {

Inotify::Inotify()
    : mError(0)
    , mEventTimeout(0)
    , mLastEventTime(std::chrono::steady_clock::now())
    , mEventMask(IN_ALL_EVENTS)
    , mThreadSleep(250)
    , mIgnoredDirectories(std::vector<std::string>())
    , mInotifyFd(0)
    , mOnEventTimeout([](FileSystemEvent) {})
{

    // Initialize inotify
    init();
}

Inotify::~Inotify()
{
    if (!close(mInotifyFd)) {
        mError = errno;
    }
}

void Inotify::init()
{
    stopped = false;
    mInotifyFd = inotify_init1(IN_NONBLOCK);
    if (mInotifyFd == -1) {
        mError = errno;
        std::stringstream errorStream;
        errorStream << "Can't initialize inotify ! " << strerror(mError) << ".";
        throw std::runtime_error(errorStream.str());
    }
}

/**
 * @brief Adds the given path and all files and subdirectories
 *        to the set of watched files/directories.
 *        Symlinks will be followed!
 *
 * @param path that will be watched recursively
 *
 */
void Inotify::watchDirectoryRecursively(std::string path)
{
    /* XXX
    if (isExists(path))
        if (isDirectory(path)) {
            fs::recursive_directory_iterator it(path, fs::symlink_option::recurse);
            fs::recursive_directory_iterator end;

            while (it != end) {
                fs::path currentPath = *it;

                if (isDirectory(currentPath)) {
                    watchFile(currentPath);
                }
                if (fs::is_symlink(currentPath)) {
                    watchFile(currentPath);
                }
                ++it;
            }
        }
        watchFile(path);
    } else {
        throw std::invalid_argument(
            "Can´t watch Path! Path does not exist. Path: " + path.string());
    }
    */
}

/**
 * @brief Adds a single file/directorie to the list of
 *        watches. Path and corresponding watchdescriptor
 *        will be stored in the directorieMap. This is done
 *        because events on watches just return this
 *        watchdescriptor.
 *
 * @param path that will be watched
 *
 */
void Inotify::watchFile(std::string filePath)
{
    if (isExists(filePath)) {
        mError = 0;
        int wd = 0;
        if (!isIgnored(filePath)) {
            wd = inotify_add_watch(mInotifyFd, filePath.c_str(), mEventMask);
        }

        if (wd == -1) {
            mError = errno;
            std::stringstream errorStream;
            if (mError == 28) {
                errorStream << "Failed to watch! " << strerror(mError)
                            << ". Please increase number of watches in "
                               "\"/proc/sys/fs/inotify/max_user_watches\".";
                throw std::runtime_error(errorStream.str());
            }

            errorStream << "Failed to watch! " << strerror(mError)
                        << ". Path: " << filePath;
            throw std::runtime_error(errorStream.str());
        }
        mDirectorieMap.emplace(wd, filePath);
    } else {
        throw std::invalid_argument(
            "Can´t watch Path! Path does not exist. Path: " + filePath);
    }
}

void Inotify::ignoreFileOnce(std::string file)
{
    mOnceIgnoredDirectories.push_back(file);
}

void Inotify::ignoreFile(std::string file)
{
    mIgnoredDirectories.push_back(file);
}


void Inotify::unwatchFile(std::string file)
{
    auto const itFound = std::find_if(std::begin(mDirectorieMap), std::end(mDirectorieMap),
                                      [&](std::pair<int, std::string> const& KeyString)
                                      { return KeyString.second == file; });

    if (itFound != std::end(mDirectorieMap))
        removeWatch(itFound->first);
}

/**
 * @brief Removes watch from set of watches. This
 *        is not done recursively!
 *
 * @param wd watchdescriptor
 *
 */
void Inotify::removeWatch(int wd)
{
    int result = inotify_rm_watch(mInotifyFd, wd);
    if (result == -1) {
        mError = errno;
        std::stringstream errorStream;
        errorStream << "Failed to remove watch! " << strerror(mError) << ".";
        throw std::runtime_error(errorStream.str());
    }
}

std::string Inotify::wdToPath(int wd)
{
    return mDirectorieMap.at(wd);
}

void Inotify::setEventMask(uint32_t eventMask)
{
    mEventMask = eventMask;
}

uint32_t Inotify::getEventMask()
{
    return mEventMask;
}

void Inotify::setEventTimeout(
    std::chrono::milliseconds eventTimeout, std::function<void(FileSystemEvent)> onEventTimeout)
{
    mEventTimeout = eventTimeout;
    mOnEventTimeout = onEventTimeout;
}

/**
 * @brief Blocking wait on new events of watched files/directories
 *        specified on the eventmask. FileSystemEvents
 *        will be returned one by one. Thus this
 *        function can be called in some while(true)
 *        loop.
 *
 * @return A new FileSystemEvent
 *
 */
TFileSystemEventPtr Inotify::getNextEvent()
{
    int length = 0;
    char buffer[EVENT_BUF_LEN];
    std::chrono::steady_clock::time_point currentEventTime;
    std::vector<FileSystemEvent> events;

    // Read Events from fd into buffer
    while (mEventQueue.empty()) {
        length = 0;
        memset(buffer, '\0', sizeof(buffer));
        while (length <= 0 && !stopped) {
            std::this_thread::sleep_for(std::chrono::milliseconds(mThreadSleep));

            length = read(mInotifyFd, buffer, EVENT_BUF_LEN);
            if (length == -1) {
                mError = errno;
                if (mError != EINTR) {
                    continue;
                }
            }
        }

        if (stopped) {
            return nullptr;
        }

        // Read events from buffer into queue
        currentEventTime = std::chrono::steady_clock::now();
        int i = 0;
        while (i < length) {
            inotify_event* event = ((struct inotify_event*)&buffer[i]);

            if(event->mask & IN_IGNORED){
                i += EVENT_SIZE + event->len;
                mDirectorieMap.erase(event->wd);
                continue;
            }
            auto path = wdToPath(event->wd) + std::string("/") + std::string(event->name);

            if (isDirectory(path)) {
                event->mask |= IN_ISDIR;
            }
            FileSystemEvent fsEvent(event->wd, event->mask, path);

            if (!fsEvent.path.empty()) {
                events.push_back(fsEvent);

            } else {
                // Event is not complete --> ignore
            }

            i += EVENT_SIZE + event->len;
        }

        // Filter events
        for (auto eventIt = events.begin(); eventIt < events.end(); ++eventIt) {
            FileSystemEvent currentEvent = *eventIt;
            if (onTimeout(currentEventTime)) {
                events.erase(eventIt);
                mOnEventTimeout(currentEvent);

            } else if (isIgnored(currentEvent.path)) {
                events.erase(eventIt);
            } else {
                mLastEventTime = currentEventTime;
                mEventQueue.push(currentEvent);
            }
        }
    }

    // Return next event
    auto event = std::make_shared<FileSystemEvent>(mEventQueue.front());
    mEventQueue.pop();
    return event;
}

void Inotify::stop()
{
    stopped = true;
}

bool Inotify::hasStopped()
{
  return stopped;
}

bool Inotify::isIgnored(std::string file)
{
    for (unsigned i = 0; i < mOnceIgnoredDirectories.size(); ++i) {
        size_t pos = file.find(mOnceIgnoredDirectories[i]);
        if (pos != std::string::npos) {
            mOnceIgnoredDirectories.erase(mOnceIgnoredDirectories.begin() + i);
            return true;
        }
    }

    for (unsigned i = 0; i < mIgnoredDirectories.size(); ++i) {
        size_t pos = file.find(mIgnoredDirectories[i]);
        if (pos != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool Inotify::onTimeout(const std::chrono::steady_clock::time_point& eventTime)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(eventTime - mLastEventTime) < mEventTimeout;
}

bool Inotify::isDirectory(const std::string& path) const
{
    if (access(path.c_str(), F_OK) != -1)
    {
        // file exists
        DIR *dirptr;
        if ((dirptr = opendir(path.c_str())) != NULL) {
            closedir(dirptr);
            return true;
        }
    }
    return false;
}
bool Inotify::isExists(const std::string& path) const
{
    return (access(path.c_str(), F_OK) != -1);
}
}
