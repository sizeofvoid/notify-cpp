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

#include <linux/version.h>
#include <sys/fanotify.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace notifycpp {

Fanotify::Fanotify()
    : Notify()
{
    initFanotify();
}

Fanotify::~Fanotify()
{
    close(_FanotifyFd);
}

void Fanotify::initFanotify()
{
/**
 * Linux Kernel < 3.15.0 workaround
 * https://github.com/torvalds/linux/commit/1e2ee49f7f1b79f0b14884fe6a602f0411b39552
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 15, 0)
    _FanotifyFd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY | 0100000);
#else
    _FanotifyFd
        = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY | O_LARGEFILE);
#endif

    if (_FanotifyFd == -1) {
        std::stringstream errorStream;
        errorStream << "Couldn't setup new fanotify device: " << strerror(errno) << ".";
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
void Fanotify::watchMountPoint(const FileSystemEvent& fse)
{
    watch(fse.getPath(), FAN_MARK_ADD | FAN_MARK_MOUNT);
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
void Fanotify::watchFile(const FileSystemEvent& fse)
{
    if (checkWatchFile(fse))
        watch(fse.getPath(), FAN_MARK_ADD, fse.getEvent());
}

void Fanotify::watch(const std::filesystem::path& path, unsigned int flags, const Event event)
{
    if (std::filesystem::exists(path)) {
        /* Add new fanotify mark */
        if (fanotify_mark(_FanotifyFd, flags, getEventMask(event), AT_FDCWD, path.c_str()) < 0) {
            std::stringstream errorStream;
            errorStream << "Couldn't add monitor '" << path << "': " << strerror(errno);
            throw std::runtime_error(errorStream.str());
        }
    } else {
        throw std::invalid_argument("Can´t watch Path! Path does not exist. Path: " + path.string());
    }
}

/**
 * @brief Removes watch from set of watches. This
 *        is not done recursively!
 *
 * @param wd watchdescriptor
 *
 */
void Fanotify::unwatch(const FileSystemEvent& fse)
{
    /* Add new fanotify mark */
    if (fanotify_mark(_FanotifyFd, FAN_MARK_REMOVE, getEventMask(fse.getEvent()), AT_FDCWD, fse.getPath().c_str()) < 0) {
        std::stringstream errorStream;
        errorStream << "Couldn't remove monitor '" << fse.getPath() << "': " << strerror(errno);
        throw std::runtime_error(errorStream.str());
    }
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
TFileSystemEventPtr Fanotify::getNextEvent()
{
    struct pollfd fds[FD_POLL_MAX];
    /* Setup polling */
    fds[FD_POLL_FANOTIFY].fd = _FanotifyFd;
    fds[FD_POLL_FANOTIFY].events = POLLIN;

    /* Now loop */
    while (_Queue.empty() && !_Stopped) {
        /* Block until there is something to be read */
        if (poll(fds, FD_POLL_MAX, -1) < 0) {
            std::stringstream errorStream;
            errorStream << "Couldn't poll(): " << strerror(errno) << ".";
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
                        //_Queue.push(std::make_shared<FileSystemEvent>(metadata->mask, filename));
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
std::uint32_t
Fanotify::getEventMask(const Event e) const
{
    return 0;
}
}
