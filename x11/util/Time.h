#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <time.h>

namespace BR {

class Time {
    timespec cur;

  public:
    Time(const timespec &cur_) : cur(cur_) {}

    Time(time_t nanoSeconds) {
        clock_gettime(CLOCK_MONOTONIC, &cur);

        uint32_t sec = nanoSeconds / 1'000'000'000;
        uint32_t nsec = nanoSeconds % 1'000'000'000;

        cur.tv_sec += sec;
        cur.tv_nsec += nsec;

        if (cur.tv_nsec >= 1'000'000'000) {
            cur.tv_sec++;
            cur.tv_nsec -= 1'000'000'000;
        }
    }

    operator const timespec &() const { return cur; }

    bool operator==(const Time &ref) const {
        return ref.cur.tv_sec == cur.tv_sec && ref.cur.tv_nsec == cur.tv_nsec;
    }
};

timespec diff(const timespec &ref, const timespec &now);

} // namespace BR
