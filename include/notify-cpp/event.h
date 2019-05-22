#pragma once

#include <cstdint>
#include <iostream>

#include <sys/fanotify.h>

namespace notifycpp {

/* All these defined in fanotify.h. */
enum class Event : std::uint64_t {
    /* File was accessed */
    access = FAN_ACCESS,

    /* File was opened */
    open = FAN_OPEN,

    /* File was modified */
    modify = FAN_MODIFY,

    /* Writtable file closed */
    close_write = FAN_CLOSE_WRITE,

    /* Unwrittable file closed */
    close_nowrite = FAN_CLOSE_NOWRITE,

    /* Event queued overflowed */
    q_overflow = FAN_Q_OVERFLOW,

    /* File open in perm check */
    perm_open = FAN_OPEN_PERM,

    /* File accessed in perm check */
    perm_access = FAN_ACCESS_PERM,

    /* close = close write or close no_write */
    close = FAN_CLOSE,
};

Event operator|(Event lhs, Event rhs);
Event operator&(Event lhs, Event rhs);
std::ostream& operator<<(std::ostream&, const Event&);
std::string toString(const Event&);
}
