/*
 * Copyright (c) 2020 Rafael Sadowski <rafael@sizeofvoid.org>
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

#include <notify-cpp/kqueue.h>

#include <notify-cpp/kqueue_event.h>
#include <notify-cpp/notify.h>

#include <sstream>
#include <iostream>
#include <string>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace notifycpp {

Kqueue::Kqueue()
    : Notify(std::shared_ptr<KqueueEventHandler>(new KqueueEventHandler))
{
    setup();
}

Kqueue::~Kqueue()
{
    close(_KqueueFd);
}

void Kqueue::setup()
{
    _KqueueFd = kqueue();

    if (_KqueueFd == -1) {
        std::stringstream errorStream;
        errorStream << "Couldn't setup kernel event queue: " << strerror(errno) << ".";
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
void Kqueue::watchMountPoint(const FileSystemEvent& fse)
{
}

std::filesystem::path
Kqueue::wdToPath(int wd) const
{
    return mDirectorieMap.at(wd);
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
void Kqueue::watchFile(const FileSystemEvent& fse)
{
    if (!checkWatchFile(fse))
        return;

    std::cout << "OK" << std::endl;
    struct kevent change;
    const int wd = open(fse);
    EV_SET(&change, wd, EVFILT_VNODE,
                        EV_ADD | EV_ENABLE,
                        NOTE_WRITE | NOTE_ATTRIB,
                        //_EventHandler->convertToEvents(fse.getEvent()),
                        0, 0);
    if (kevent(_KqueueFd, &change, 1, NULL, 0, NULL) == -1)
        std::cout << "Failed: " << fse.getPath() << std::endl;

    std::cout << "OK: " << fse.getPath() << std::endl;
    std::cout << "add wd: " << wd << std::endl;
    mDirectorieMap.emplace(wd, fse.getPath());
}

/**
 *
 */
void Kqueue::unwatch(const FileSystemEvent& fse)
{
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
TFileSystemEventPtr Kqueue::getNextEvent()
{
    int waitms = 1000;
        struct timespec timeout;
            timeout.tv_sec = 0;
                timeout.tv_nsec = 500;
    const int maxEvents = 20;
    struct kevent events[maxEvents];
    std::cout << "getNextEvent: "<< std::endl;
    /* Now loop */
    while (_Queue.empty() && isRunning()) {
        std::cout << "getNextEvent: "<< std::endl;
        const int nev = kevent(_KqueueFd, NULL, 0, events, maxEvents, &timeout);   /* block indefinitely */
        std::cout << "getNextEvent: nev: " << nev << std::endl;
        if (nev == -1) {
            std::stringstream errorStream;
            errorStream << "kevent(): " << strerror(errno) << ".";
            throw std::runtime_error(errorStream.str());
        }
        else if (nev > 0) {
            for (int i = 0; i < nev; i++) {
                if (events[i].flags & EV_ERROR) {
                    std::stringstream errorStream;
                    errorStream << "EV_ERROR: " << strerror(events[i].data) << ".";
                    throw std::runtime_error(errorStream.str());
                }

                const int file_events = events[i].filter;
                const int fd = (int)(intptr_t)events[i].ident;
                if (fd <= 0)
                    continue;
                std::cout << "add fd: " << fd << std::endl;
                const std::filesystem::path path = wdToPath(fd);

                if (!isIgnoredOnce(path)) {
                    _Queue.push(std::make_shared<FileSystemEvent>(path,
                        _EventHandler->get(
                            static_cast<uint32_t>(file_events))));
                }
            }
        }
    }

    if (isStopped() || _Queue.empty()) {
        return nullptr;
    }

    // Return next event
    auto e = _Queue.front();
    _Queue.pop();
    return e;
}

std::uint32_t
Kqueue::getEventMask(const Event event) const
{
    //return _EventHandler->convertEvents(event);
    return 0;
}

int Kqueue::open(const FileSystemEvent& fse)
{
    const int fd = ::open(fse.getPath().c_str(), O_RDONLY);

    if (fd == -1) {
        std::stringstream errorStream;
        errorStream << "Couldn't open " << fse.getPath() << " :" << strerror(errno) << ".";
        throw std::runtime_error(errorStream.str());
    }
    return fd;
}
}
