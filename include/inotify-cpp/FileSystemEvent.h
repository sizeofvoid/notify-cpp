#pragma once

#include <memory>
#include <string>
#include <vector>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(uint64_t, const std::string);
    ~FileSystemEvent();

  public:
    uint64_t mask;
    std::string path;
};
using TFileSystemEventPtr = std::shared_ptr<FileSystemEvent>;
}
