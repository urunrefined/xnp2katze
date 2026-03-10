#include "Time.h"
#include <ctime>

namespace BR {

timespec diff(const timespec &ref, const timespec &now) {

    if (ref.tv_sec < now.tv_sec)
        return {0, 0};

    if (ref.tv_sec == now.tv_sec) {
        if (ref.tv_nsec <= now.tv_nsec) {
            return {0, 0};
        } else {
            return {0, ref.tv_nsec - now.tv_nsec};
        }
    }

    time_t secs = ref.tv_sec - now.tv_sec;

    if (ref.tv_nsec < now.tv_nsec) {
        secs--;

        return {secs, (ref.tv_nsec + 1'000'000'000) - now.tv_nsec};
    } else {
        return {secs, ref.tv_nsec - now.tv_nsec};
    }
}

} // namespace BR
