#pragma once

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include <notify-cpp/event.h>

namespace notifycpp {
class FileSystemEvent {
  public:
    FileSystemEvent(const std::filesystem::path&);
    FileSystemEvent(const std::filesystem::path&,
                    const Event);
    ~FileSystemEvent();

    Event getEvent() const;
    std::filesystem::path getPath() const;

  private:
    //!
    Event _Event;

    //! absoulte path + filename
    std::filesystem::path _Path;
};
using TFileSystemEventPtr = std::shared_ptr<FileSystemEvent>;
}
