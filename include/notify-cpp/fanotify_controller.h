#pragma once

#include <notify-cpp/notify_controller.h>

namespace notifycpp {

class FanotifyController : public NotifyController {
public:
    FanotifyController();

    NotifyController& watchMountPoint(const std::filesystem::path&);
};
}
