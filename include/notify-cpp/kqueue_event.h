#pragma once

#include <notify-cpp/event.h>

namespace notifycpp {

class KqueueEventHandler : public EventHandler {
public:
    KqueueEventHandler(const Event);
    KqueueEventHandler() = default;
    ~KqueueEventHandler() = default;

    virtual std::uint32_t convertToEvents(const Event) const override;
    virtual std::uint32_t getEvent(const Event) const override;
    virtual std::vector<Event> getEvents(std::uint32_t) const override;
    virtual Event get(std::uint32_t) const override;
    virtual std::string toString(std::uint32_t) const override;
};
}
