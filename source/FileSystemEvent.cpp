#include <inotify-cpp/FileSystemEvent.h>

#include <sys/inotify.h>

namespace inotify {
FileSystemEvent::FileSystemEvent(uint64_t mask, const std::string path)
    : mask(mask)
    , path(path)
{
}

FileSystemEvent::~FileSystemEvent()
{
}
}
