/*
 * Copyright (c) 2019 Rafael Sadowski <rafael@sizeofvoid.org>
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

#include <notify-cpp/notify_controller.h>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <filesystem>
#include <future>
#include <fstream>

using namespace notifycpp;

void openFile(const std::filesystem::path& file)
{
    std::ofstream stream;
    stream.open(file.string(), std::ifstream::out);
    BOOST_CHECK(stream.is_open());
    stream << "Writing this to a file.\n";
    stream.close();
}

struct FilesystemEventHelper {
    FilesystemEventHelper()
            : testDirectory_("testDirectory")
            , recursiveTestDirectory_(testDirectory_ / "recursiveTestDirectory")
            , testFileOne_(testDirectory_ / "test.txt")
            , testFileTwo_(testDirectory_ / "test2.txt")
            , timeout_(1)
    {
        std::filesystem::create_directories(testDirectory_);
        std::ofstream streamOne(testFileOne_);
        std::ofstream streamTwo(testFileTwo_);
    }

    ~FilesystemEventHelper() = default;

    std::filesystem::path testDirectory_;
    std::filesystem::path recursiveTestDirectory_;
    std::filesystem::path testFileOne_;
    std::filesystem::path testFileTwo_;

    std::chrono::seconds timeout_;

    // Events
    std::promise<size_t> _promisedCounter;
    std::promise<Notification> promisedOpen_;
    std::promise<Notification> promisedCloseNoWrite_;
};

