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

#pragma once

#include <notify-cpp/file_system_event.h>
#include <notify-cpp/notify.h>

#include <filesystem>

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
namespace notifycpp {

class Fanotify : public Notify {
    enum { FD_POLL_FANOTIFY = 0, FD_POLL_MAX };

  public:
    Fanotify();
    ~Fanotify();

    virtual void watchMountPoint(const FileSystemEvent&);
    virtual void watchFile(const FileSystemEvent&) override;
    virtual void unwatch(const FileSystemEvent&) override;
    virtual TFileSystemEventPtr getNextEvent() override;
    virtual std::uint32_t getEventMask(const Event) const override;

  private:
    void initFanotify();
    void watch(const std::filesystem::path&, unsigned int, const Event = Event::open);

    int _FanotifyFd = -1;
};
}
