#pragma once
#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdio>
namespace vregex {

#if __cplusplus < 202302L
template <std::integral T> constexpr T byteswap(T x) {

    constexpr size_t n = sizeof(T);
    std::byte *const bytes = (std::byte *)(&x);
    for (size_t i = 0; i < n / 2; i++) {
        std::swap(bytes[i], bytes[n - i - 1]);
    }
    return x;
}
#else
using std::byteswap;
#endif
}; // namespace vregex