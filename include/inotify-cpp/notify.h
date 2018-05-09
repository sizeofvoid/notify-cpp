/*
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

#pragma once

#include <inotify-cpp/FileSystemEvent.h>

#include <atomic>
#include <queue>
#include <string>
#include <vector>

/**
 * @brief Base class
 */
namespace inotify {

class Notify {

  public:
    Notify();

    virtual void watchMountPoint(const std::string&) = 0;
    virtual void watchFile(const std::string&) = 0;
    virtual void unwatch(const std::string&) = 0;

    virtual TFileSystemEventPtr getNextEvent() = 0;

    void stop();
    bool hasStopped();

    void setEventMask(uint64_t);
    uint64_t getEventMask();
    void ignoreFile(const std::string&);

  protected:
    bool isIgnored(const std::string&);
    void initFanotify();
    bool isDirectory(const std::string&) const;
    bool isExists(const std::string&) const;
    std::string getFilePath(int) const;

    uint64_t _EventMask = 0;

    std::vector<std::string> _IgnoredDirectories;

    std::queue<TFileSystemEventPtr> _Queue;

    std::atomic<bool> _Stopped;
};
}
