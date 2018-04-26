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

#include <inotify-cpp/Inotify.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/fanotify.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace inotify {

Inotify::Inotify()
    : _Error(0)
    , _EventMask(FAN_ALL_EVENTS)
{
    initFanotify();
    initSignals();
}

Inotify::~Inotify()
{
    if (!close(_FanotifyFd)) {
        _Error = errno;
    }
    if (!close(_SignalFd)) {
        _Error = errno;
    }
}

void Inotify::initFanotify()
{
    _Stopped = false;
    _FanotifyFd
        = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY | O_LARGEFILE);

    if (_FanotifyFd == -1) {
        _Error = errno;
        std::stringstream errorStream;
        errorStream << "Couldn't setup new fanotify device: " << strerror(_Error) << ".";
        throw std::runtime_error(errorStream.str());
    }
}

void Inotify::initSignals()
{
    _Stopped = false;
    sigset_t sigmask;

    /* We want to handle SIGINT and SIGTERM in the _SignalFd, so we block them. */
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);
    sigaddset(&sigmask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) < 0) {
        _Error = errno;
        std::stringstream errorStream;
        errorStream << "Couldn't block signals: " << strerror(_Error) << ".";
        throw std::runtime_error(errorStream.str());
    }

    /* Get new FD to read signals from it */
    if ((_SignalFd = signalfd(-1, &sigmask, 0)) < 0) {
        _Error = errno;
        std::stringstream errorStream;
        errorStream << "Couldn't setup signal FD: " << strerror(_Error) << ".";
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
void Inotify::watchMountPoint(std::string path)
{
    watch(path, FAN_MARK_ADD | FAN_MARK_MOUNT);
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
    watch(filePath, FAN_MARK_ADD);
}

void Inotify::watch(std::string path, unsigned int flags)
{
    if (isExists(path)) {
        _Error = 0;

        /* Add new fanotify mark */
        if (fanotify_mark(_FanotifyFd, flags, getEventMask(), AT_FDCWD, path.c_str()) < 0) {
            _Error = errno;
            std::stringstream errorStream;
            errorStream << "Couldn't add monitor '" << path << "': " << strerror(_Error);
            throw std::runtime_error(errorStream.str());
        }
    } else {
        throw std::invalid_argument("Can´t watch Path! Path does not exist. Path: " + path);
    }
}

void Inotify::ignoreFile(std::string file)
{
    _IgnoredDirectories.push_back(file);
}

/**
 * @brief Removes watch from set of watches. This
 *        is not done recursively!
 *
 * @param wd watchdescriptor
 *
 */
void Inotify::unwatch(const std::string& path)
{
    _Error = 0;
    /* Add new fanotify mark */
    if (fanotify_mark(_FanotifyFd, FAN_MARK_REMOVE, getEventMask(), AT_FDCWD, path.c_str()) < 0) {
        _Error = errno;
        std::stringstream errorStream;
        errorStream << "Couldn't remove monitor '" << path << "': " << strerror(_Error);
        throw std::runtime_error(errorStream.str());
    }
}

void Inotify::setEventMask(uint64_t eventMask)
{
    _EventMask = eventMask;
}

uint64_t Inotify::getEventMask()
{
    return _EventMask;
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
    struct pollfd fds[FD_POLL_MAX];
    /* Setup polling */
    fds[FD_POLL_FANOTIFY].fd = _FanotifyFd;
    fds[FD_POLL_FANOTIFY].events = POLLIN;

    /* Now loop */
    while (_Queue.empty()) {
        /* Block until there is something to be read */
        if (poll(fds, FD_POLL_MAX, -1) < 0) {
            _Error = errno;
            std::stringstream errorStream;
            errorStream << "Couldn't poll(): " << strerror(_Error) << ".";
            throw std::runtime_error(errorStream.str());
        }

        if (_Stopped) {
            return nullptr;
        }

        /* fanotify event received? */
        if (fds[FD_POLL_FANOTIFY].revents & POLLIN) {
            char buffer[/*FANOTIFY_BUFFER_SIZE*/ 8192];
            ssize_t length;

            /* Read from the FD. It will read all events available up to
             * the given buffer size. */
            if ((length = read(fds[FD_POLL_FANOTIFY].fd, buffer, /*FANOTIFY_BUFFER_SIZE*/ 8192))
                > 0) {
                struct fanotify_event_metadata* metadata;

                metadata = (struct fanotify_event_metadata*)buffer;

                while (FAN_EVENT_OK(metadata, length)) {

                    const std::string filename = getFilePath(metadata->fd);
                    if (!filename.empty()) {
                        // TODO Filter events
                        _Queue.push(std::make_shared<FileSystemEvent>(metadata->mask, filename));
                        close(metadata->fd);
                    }
                    metadata = FAN_EVENT_NEXT(metadata, length);
                }
            }
        }
    }
    // Return next event
    auto event = _Queue.front();
    _Queue.pop();
    return event;
}

void Inotify::stop()
{
    _Stopped = true;
}

bool Inotify::hasStopped()
{
    return _Stopped;
}

bool Inotify::isIgnored(std::string file)
{
    for (unsigned i = 0; i < _IgnoredDirectories.size(); ++i) {
        size_t pos = file.find(_IgnoredDirectories[i]);
        if (pos != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool Inotify::isDirectory(const std::string& path) const
{
    if (access(path.c_str(), F_OK) != -1) {
        // file exists
        DIR* dirptr;
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

std::string Inotify::getFilePath(int fd) const
{
    ssize_t len;
    char buffer[PATH_MAX];

    if (fd <= 0)
        return {};

    sprintf(buffer, "/proc/self/fd/%d", fd);
    if ((len = readlink(buffer, buffer, PATH_MAX - 1)) < 0)
        return {};

    buffer[len] = '\0';
    return std::string(buffer);
}
}
