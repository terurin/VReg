#pragma once
#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <endian.h>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
namespace vreg::base {

using size_opt = std::optional<size_t>;
using addr_t = uint32_t;

template <class W>
concept writer_at = requires(W &writer, addr_t addr, std::span<const std::byte> bytes, std::endian endian) {
    { writer(addr, bytes, endian) } -> std::same_as<size_opt>;
};

template <class R>
concept reader_at = requires(R &reader, addr_t addr, std::span<std::byte> bytes, std::endian endian) {
    { reader(addr, bytes, endian) } -> std::same_as<size_opt>;
};

template <class W>
concept writer = requires(W &writer, std::span<const std::byte> bytes, std::endian endian) {
    { writer(bytes, endian) } -> std::same_as<size_opt>;
};

template <class R>
concept reader = requires(R &reader, std::span<std::byte> bytes, std::endian endian) {
    { reader(bytes, endian) } -> std::same_as<size_opt>;
};

struct VMountBase {
    const std::string name_; // for auto documentation
    const std::string desc_; // for auto documentation
    VMountBase(std::string_view name, std::string_view desc) : name_(name), desc_(desc) {}
    VMountBase(const VMountBase &) = default;
    virtual ~VMountBase() = default;
    virtual size_opt writeAt(addr_t addr, std::span<const std::byte>, std::endian endian = std::endian::native) = 0;
    virtual size_opt readAt(addr_t addr, std::span<std::byte>, std::endian endian = std::endian::native) = 0;
    virtual size_t size() const = 0;
    size_t mask() const { return std::bit_ceil(size()) - 1; }
    size_t maskWidth() const { return std::bit_width(size()); }
    bool has(size_t addr) const { return addr < size(); }
};

struct VRegBase : public VMountBase {
    VRegBase(std::string_view name, std::string_view desc = "") : VMountBase(name, desc) {}
    VRegBase(const VRegBase &) = default;
    VRegBase(VRegBase &&) = default;
    virtual ~VRegBase() = default;
    virtual size_t size() const { return 1; }
    virtual size_opt write(std::span<const std::byte> bytes, std::endian endian = std::endian::native) {
        (void)bytes, (void)endian;
        return std::nullopt;
    }
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) {
        (void)bytes, (void)endian;
        return std::nullopt;
    }
    virtual size_opt writeAt(addr_t addr, std::span<const std::byte> bytes,
                             std::endian endian = std::endian::native) override {
        return (addr == 0) ? write(bytes, endian) : std::nullopt;
    };
    virtual size_opt readAt(addr_t addr, std::span<std::byte> bytes, std::endian endian = std::endian::native) {
        return (addr == 0) ? read(bytes, endian) : std::nullopt;
    };
};

using VRegBasePtr = std::shared_ptr<VRegBase>;
using VMountBasePtr = std::shared_ptr<VMountBase>;

}; // namespace vreg::base