/**
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      20.11.2017
 * @copyright MIT
 **/
#pragma once
#include <assert.h>
#include <atomic>
#include <chrono>
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
#include <sys/fanotify.h>

#define MAX_EVENTS 4096
#define EVENT_SIZE (sizeof(inotify_event))
#define EVENT_BUF_LEN (MAX_EVENTS * (EVENT_SIZE + 16))

/**
 * @brief C++ wrapper for linux fanotify interface
 *
 * folders will be watched by wa
 * folders will be watched by watchFolderRecursively or
 * files by watchFile. If there are changes inside this
 * folder or files events will be raised. This events
 * can be get by getNextEvent.
 *
 * @eventMask @see Event2
 *
 * See fanotify manpage for more event details
 *
 */
namespace inotify {

class Inotify {
    enum { FD_POLL_FANOTIFY = 0, FD_POLL_MAX };

  public:
    Inotify();
    ~Inotify();
    void watchMountPoint(std::string);
    void watchFile(std::string);
    void unwatch(const std::string&);
    void ignoreFile(std::string);
    void setEventMask(uint64_t);
    uint64_t getEventMask();
    TFileSystemEventPtr getNextEvent();
    void stop();
    bool hasStopped();

  private:
    /* Enumerate list of FDs to poll */

    void watch(std::string, unsigned int);
    std::string wdToPath(int wd);
    bool isIgnored(std::string file);

    void initFanotify();
    void initSignals();

    // Member
    int _Error;
    uint64_t _EventMask;
    std::vector<std::string> _IgnoredDirectories;
    std::queue<TFileSystemEventPtr> _Queue;

    int _FanotifyFd = -1;
    int _SignalFd = -1;
    std::atomic<bool> _Stopped;

    bool isDirectory(const std::string&) const;
    bool isExists(const std::string&) const;
    std::string getFilePath(int) const;
};
}
