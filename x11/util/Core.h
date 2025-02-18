#pragma once

#include <stddef.h>
#include <stdint.h>

namespace BR {

template <typename T1, size_t N> constexpr size_t arraySize(T1 (&)[N]) {
    return N;
}

template <typename T1, size_t N> class ArrayNTOs {
  public:
    static_assert(N != 0);
    const size_t size = N;
    alignas(alignof(T1)) char data[sizeof(T1) * N];

    template <typename... Args> ArrayNTOs(Args &&...args) {
        for (size_t i = 0; i < N; i++) {
            new (((T1 *)data) + i) T1(args...);
        }
    }

    ~ArrayNTOs() {
        for (size_t i = N - 1; i < N; i--) {
            (((T1 *)data)[i].~T1());
        }
    }

    T1 &operator[](size_t i) { return ((T1 *)data)[i]; }

    const T1 &operator[](size_t i) const { return ((T1 *)data)[i]; }
};

} // namespace BR
