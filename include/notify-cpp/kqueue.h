/**
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

#pragma once

#include <notify-cpp/file_system_event.h>
#include <notify-cpp/notify.h>

#include <filesystem>
#include <map>

/**
 * @brief C++ wrapper for kqueue interface
 */
namespace notifycpp {

class Kqueue : public Notify {
public:
    Kqueue();
    virtual ~Kqueue();

    virtual void watchMountPoint(const FileSystemEvent&);
    virtual void watchFile(const FileSystemEvent&) override;
    virtual void unwatch(const FileSystemEvent&) override;
    virtual TFileSystemEventPtr getNextEvent() override;
    virtual std::uint32_t getEventMask(const Event) const override;
private:
    void setup();
    int open(const FileSystemEvent&);
    std::filesystem::path wdToPath(int) const;
    int _KqueueFd = -1;
    std::map<int, std::filesystem::path> mDirectorieMap;
};
}
