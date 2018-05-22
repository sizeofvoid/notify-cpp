/**
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      20.11.2017
 * @copyright MIT
 **/
#pragma once
#include <assert.h>
#include <atomic>
#include <errno.h>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <sys/inotify.h>
#include <thread>
#include <time.h>
#include <vector>

#include <inotify-cpp/FileSystemEvent.h>
#include <inotify-cpp/notify.h>

#define MAX_EVENTS 4096
#define EVENT_SIZE (sizeof(inotify_event))
#define EVENT_BUF_LEN (MAX_EVENTS * (EVENT_SIZE + 16))

/**
 * @brief C++ wrapper for linux inotify interface
 * @class Inotify
 *        Inotify.h
 *        "include/Inotify.h"
 *
 * folders will be watched by watchFolderRecursively or
 * files by watchFile. If there are changes inside this
 * folder or files events will be raised. This events
 * can be get by getNextEvent.
 *
 * @eventMask
 *
 * IN_ACCESS         File was accessed (read) (*).
 * IN_ATTRIB         Metadata changed—for example, permissions,
 *                   timestamps, extended attributes, link count
 *                   (since Linux 2.6.25), UID, or GID. (*).
 * IN_CLOSE_WRITE    File opened for writing was closed (*).
 * IN_CLOSE_NOWRITE  File not opened for writing was closed (*).
 * IN_CREATE         File/directory created in watched directory(*).
 * IN_DELETE         File/directory deleted from watched directory(*).
 * IN_DELETE_SELF    Watched file/directory was itself deleted.
 * IN_MODIFY         File was modified (*).
 * IN_MOVE_SELF      Watched file/directory was itself moved.
 * IN_MOVED_FROM     Generated for the directory containing the old
 *                   filename when a file is renamed (*).
 * IN_MOVED_TO       Generated for the directory containing the new
 *                   filename when a file is renamed (*).
 * IN_OPEN           File was opened (*).
 * IN_ALL_EVENTS     macro is defined as a bit mask of all of the above
 *                   events
 * IN_MOVE           IN_MOVED_FROM|IN_MOVED_TO
 * IN_CLOSE          IN_CLOSE_WRITE | IN_CLOSE_NOWRITE
 *
 * See inotify manpage for more event details
 *
 */
namespace inotify {

class Inotify : public Notify {
  public:
    Inotify();
    ~Inotify();
    virtual void watchMountPoint(const std::string&) override;
    virtual void watchFile(const std::string&) override;
    virtual void unwatch(const std::string&) override;
    virtual TFileSystemEventPtr getNextEvent() override;

  private:
    std::string wdToPath(int wd);
    void removeWatch(int wd);
    void init();

    // Member
    int mError;
    uint32_t mEventMask;
    uint32_t mThreadSleep;
    std::vector<std::string> mIgnoredDirectories;
    std::vector<std::string> mOnceIgnoredDirectories;
    std::map<int, std::string> mDirectorieMap;
    int mInotifyFd;
    std::atomic<bool> stopped;
    std::function<void(FileSystemEvent)> mOnEventTimeout;
};
}
