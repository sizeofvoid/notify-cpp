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

#include <notify-cpp/notify.h>

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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace notifycpp {

Notify::Notify()
    : _Stopped(false)
    , mThreadSleep(250)
{
}

void Notify::ignore(const std::filesystem::path& p)
{
    _Ignored.push_back(p);
}

void Notify::ignoreOnce(const std::filesystem::path& p)
{
    _IgnoredOnce.push_back(p);
}

void Notify::stop()
{
    _Stopped = true;
}

bool Notify::hasStopped()
{
    return _Stopped;
}

bool Notify::isIgnoredOnce(const std::filesystem::path& p) const
{
    auto found = std::find(std::begin(_IgnoredOnce), std::end(_IgnoredOnce), p);
    if (found != std::end(_IgnoredOnce)) {
        _IgnoredOnce.erase(found);
        return true;
    }
    return false;
}

bool Notify::isIgnored(const std::filesystem::path& p) const
{
    return std::any_of(std::begin(_Ignored), std::end(_Ignored),
        [&p](const std::filesystem::path& ip) { return p == ip; });
}

std::string Notify::getFilePath(int fd) const
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

bool Notify::checkWatchFile(const FileSystemEvent& fse) const
{
    if (!std::filesystem::exists(fse.getPath()))
        throw std::invalid_argument("Can´t watch file! File does not exist. Fullpath: " + fse.getPath().string());

    if (!std::filesystem::is_regular_file(fse.getPath())) {
        throw std::invalid_argument("Can´t watch file! Path does not refers to a regular file: " + fse.getPath().string());
    }
    return !isIgnored(fse.getPath());
}

bool Notify::checkWatchDirectory(const FileSystemEvent& fse) const
{
    if (!std::filesystem::exists(fse.getPath()))
        throw std::invalid_argument("Can´t watch Path! Path does not exist. Path: " + fse.getPath().string());

    if (!std::filesystem::is_directory(fse.getPath())) {
        throw std::invalid_argument("Can´t watch path! Path does not refers to a directory: " + fse.getPath().string());
    }
    return !isIgnored(fse.getPath());
}

void Notify::watchPathRecursively(const FileSystemEvent& fse)
{
    if (!checkWatchDirectory(fse))
        return;

    for(auto& p: std::filesystem::recursive_directory_iterator(fse.getPath())) {
        const FileSystemEvent tmp_fse(p);
        if (checkWatchFile(tmp_fse)) {
            watchFile(tmp_fse);
        }
    }
}

/**
 * @return true if Notify has stopped, otherwise false
 */
bool Notify::isStopped() const
{
    return _Stopped;
}

/**
 * @return true if Notify running and should continue, otherwise false
 */
bool Notify::isRunning() const
{
    return !_Stopped;
}

}
