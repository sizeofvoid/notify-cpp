#pragma once

#include <memory>
#include <string>
#include <vector>

namespace notifycpp {
class FileSystemEvent {
  public:
    FileSystemEvent(uint64_t, const std::string&);
    ~FileSystemEvent();

    uint64_t getMask() const;
    std::string getPath() const;

  private:
    //!
    uint64_t _Mask;

    //! absoulte path + filename
    std::string _Path;
};
using TFileSystemEventPtr = std::shared_ptr<FileSystemEvent>;
}
