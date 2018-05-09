#pragma once

#include <cstdint>
#include <iostream>

#include <sys/fanotify.h>

namespace inotify {

/* All these defined in fanotify.h. */
enum class Event : std::uint64_t {
    /* File was accessed */
    access = FAN_ACCESS,

    /* File was modified */
    modify = FAN_MODIFY,

    /* Writtable file closed */
    close_write = FAN_CLOSE_WRITE,

    /* Unwrittable file closed */
    close_nowrite = FAN_CLOSE_NOWRITE,

    /* File was opened */
    open = FAN_OPEN,

    /* Event queued overflowed */
    q_overflow = FAN_Q_OVERFLOW,

    /* File open in perm check */
    perm_open = FAN_OPEN_PERM,

    /* File accessed in perm check */
    perm_access = FAN_ACCESS_PERM,

    /* event occurred against dir */
    ondir = FAN_ONDIR,

    /* close = close write or close no_write */
    close = FAN_CLOSE,

    /* interested in child events */
    event_on_child = FAN_EVENT_ON_CHILD,
    /*
     * All of the events - we build the list by hand so that we can add flags in
     * the future and not break backward compatibility.  Apps will get only the
     * events that they originally wanted.  Be sure to add new events here!
     */
    all_events = FAN_ALL_EVENTS,

    /*
     * All events which require a permission response from userspace
     */
    all_outgoing_events = FAN_ALL_OUTGOING_EVENTS

};

Event operator|(Event lhs, Event rhs);
Event operator&(Event lhs, Event rhs);
std::ostream& operator<<(std::ostream&, const Event&);
std::string toString(const Event&);
}
