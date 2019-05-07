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
{
}

void Notify::ignore(const std::filesystem::path& p)
{
    _IgnoredDirectories.push_back(p);
}

void Notify::setEventMask(uint64_t eventMask)
{
    _EventMask = eventMask;
}

uint64_t Notify::getEventMask()
{
    return _EventMask;
}

void Notify::stop()
{
    _Stopped = true;
}

bool Notify::hasStopped()
{
    return _Stopped;
}

bool Notify::isIgnored(const std::filesystem::path& p)
{
    return std::any_of(_IgnoredDirectories.begin(), _IgnoredDirectories.end(),
                       [&p](const std::filesystem::path & ip) { return ip == p; });
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
}
