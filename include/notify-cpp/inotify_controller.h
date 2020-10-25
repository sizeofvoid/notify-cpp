#pragma once

#include <notify-cpp/notify_controller.h>

namespace notifycpp {

using EventObserver = std::function<void(Notification)>;

class InotifyController : public NotifyController {
public:
    InotifyController();
};
}
