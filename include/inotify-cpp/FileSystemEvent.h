#pragma once

#include <string>
#include <memory>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(int wd, uint32_t mask, const std::string path);

    ~FileSystemEvent();

  public: // Member
    int wd;
    uint32_t mask;
    std::string path;
};
using TFileSystemEventPtr = std::shared_ptr<FileSystemEvent>;
}
