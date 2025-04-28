#pragma once
#include <algorithm>
#include <bit>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string_view>
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

// NOTE: heapless string class like SQL's VARCHAR(n)
template <size_t cap> class varchar {
    constexpr static size_t capacity_ = cap;
    using traits = std::char_traits<char>;
    char chars_[cap + 1];

public:
    constexpr varchar() { memset(chars_, 0, sizeof(chars_)); }

    constexpr varchar(const char *s) {
        assert(s);
        const size_t l = strnlen(s, cap);
        memcpy(chars_, s, l);
        memset(chars_ + l, 0, sizeof(chars_) - l);
    }
    constexpr varchar(std::string_view sv) { sv.copy(chars_, cap); }
    template <size_t cap2> constexpr varchar(const varchar<cap2> &v) {
        const size_t l = std::min(v.size(), cap);
        memcpy(chars_, v.data(), l);
        memset(chars_ + l, 0, sizeof(chars_) - l);
    }
    constexpr varchar(varchar<cap> &&) = default;
    auto operator<=>(const varchar<cap> &) const = default;
    constexpr operator std::string_view() const { return chars_; }

    // iterator
    constexpr char *begin() { return chars_; }
    constexpr const char *cbegin() const { return chars_; }
    constexpr char *end() { return chars_ + size(); }
    constexpr const char *cend() const { return chars_ + size(); }

    // region
    constexpr size_t max_size() const { return cap; }
    constexpr size_t size() const { return traits::length(chars_); }
    constexpr size_t length() const { return size(); }
    constexpr bool empty() const { return chars_[0]; }

    // element
    constexpr char at(size_t index) const { return index < capacity_ ? chars_[index] : 0; }
    constexpr char operator[](size_t index) const { return index < sizeof(chars_) ? chars_[index] : 0; }
    constexpr char &operator[](size_t index) {
        assert(index < sizeof(chars_));
        return chars_[index];
    }
    constexpr const char *data() const { return chars_; }
    constexpr const char *c_str() const { return chars_; }

    // modify
    constexpr varchar &operator=(std::string_view sv) {
        const size_t l = std::min(sv.size(), cap);
        memcpy(chars_, sv.data(), l);
        memset(chars_ + l, 0, sizeof(chars_) - l);
        return *this;
    }
    constexpr varchar &operator+=(std::string_view sv) {
        const size_t n = size();
        const size_t writable = std::min(sv.size(), max_size() - n);
        strncpy(chars_ + n, sv.data(), writable);
        return *this;
    }
};
template <size_t n> std::ostream &operator<<(std::ostream &os, const varchar<n> &v) { return os << v.c_str(); }

using varchar_short = varchar<7>;
using varchar_middle = varchar<127>; 
using varchar_long = varchar<255>;

}; // namespace vregex

template <size_t n> struct std::hash<vregex::varchar<n>> {
    size_t operator()(const vregex::varchar<n> &v) const { return std::hash<const char *>()(v.c_str()); }
};