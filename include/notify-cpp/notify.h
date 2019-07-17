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

#include <notify-cpp/file_system_event.h>

#include <notify-cpp/event.h>

#include <atomic>
#include <filesystem>
#include <queue>
#include <string>
#include <vector>

/**
 * @brief Base class
 */
namespace notifycpp {

class Notify {

public:
    Notify();

    virtual void watchFile(const FileSystemEvent&) = 0;
    virtual void unwatch(const FileSystemEvent&) = 0;

    virtual TFileSystemEventPtr getNextEvent() = 0;

    void stop();
    bool hasStopped();

    virtual std::uint32_t getEventMask(const Event) const = 0;
    void ignore(const std::filesystem::path&);
    void ignoreOnce(const std::filesystem::path&);

    void watchPathRecursively(const FileSystemEvent&);

protected:
    bool checkWatchFile(const FileSystemEvent&) const;
    bool checkWatchDirectory(const FileSystemEvent&) const;
    bool isIgnored(const std::filesystem::path&) const;
    bool isIgnoredOnce(const std::filesystem::path&) const;
    std::string getFilePath(int) const;
    bool isStopped() const;
    bool isRunning() const;

    std::vector<std::filesystem::path> _Ignored;
    mutable std::vector<std::filesystem::path> _IgnoredOnce;

    std::queue<TFileSystemEventPtr> _Queue;

    std::atomic<bool> _Stopped;

    const uint32_t mThreadSleep;

    EventHandler _EventHandler;
};
}
