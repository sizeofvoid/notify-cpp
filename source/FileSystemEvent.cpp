#include <inotify-cpp/FileSystemEvent.h>

#include <sys/inotify.h>

namespace inotify {
FileSystemEvent::FileSystemEvent(const int wd, uint32_t mask, const std::string path)
    : wd(wd)
    , mask(mask)
    , path(path)
{
}

FileSystemEvent::~FileSystemEvent()
{
}
}
