#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace BR {

template <size_t T1> class Bitset {
  private:
    size_t size = T1;
    uint8_t bitset[T1 / 8 + ((T1 % 8))];

  public:
    Bitset() {}

    bool operator()(size_t pos) const {
        const uint8_t mod = pos % 8;
        pos /= 8;

        assert(pos < size);
        return (bitset[pos] >> mod) & 0x1;
    }

    void set(size_t pos) {
        const uint8_t mod = pos % 8;
        pos /= 8;

        assert(pos < size);
        bitset[pos] |= (char)(0x1 << mod);
    }

    void clear(size_t pos) {
        const uint8_t mod = pos % 8;
        pos /= 8;

        assert(pos < size);
        bitset[pos] &= (char)(~((0x1 << mod)));
    }

    void zero() { memset(bitset, 0, sizeof(bitset)); }
};

} // namespace BR
