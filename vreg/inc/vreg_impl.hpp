#pragma once
#include "vreg_base.hpp"
#include "vregex.hpp"

namespace vreg::impl {
using base::size_opt, base::addr_t;
using base::VRegBase, base::VRegBasePtr, base::VMountBase, base::VMountBase;
using base::writer, base::reader;

template <writer W, reader R> struct VReg : public VRegBase {
    const W writer_;
    const R reader_;
    constexpr VReg(W &&writer, R &&reader, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), writer_(std::move(writer)), reader_(std::move(reader)) {}
    virtual size_opt write(std::span<const std::byte> bytes, std::endian endian = std::endian::native) override {
        return writer_(bytes, endian);
    }
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) override {
        return reader_(bytes, endian);
    }
};

template <writer W> static inline auto VRegWO(W &&writer, std::string_view name, std::string_view desc = "") {
    auto reader = [](std::span<std::byte> bytes, std::endian endian) -> size_opt {
        (void)bytes, (void)endian;
        return std::nullopt;
    };
    return VReg(std::move(writer), std::move(reader), name, desc);
}

template <reader R> static inline auto VRegRO(R &&reader, std::string_view name, std::string_view desc = "") {
    auto writer = [](std::span<const std::byte> bytes, std::endian endian) -> size_opt {
        (void)bytes, (void)endian;
        return std::nullopt;
    };
    return VReg(std::move(writer), std::move(reader), name, desc);
}

static inline auto VRegReserved(std::string_view name="(reserved)", std::string_view desc = "") {
    auto writer = [](std::span<const std::byte> bytes, std::endian endian) -> size_opt {
        (void)bytes, (void)endian;
        return std::nullopt;
    };
    auto reader = [](std::span<std::byte> bytes, std::endian endian) -> size_opt {
        (void)bytes, (void)endian;
        return std::nullopt;
    };
    return VReg(std::move(writer), std::move(reader), name, desc);
}

template <class T> struct VRegBinder : public VRegBase {
    T &binder_;
    constexpr VRegBinder(T &binder, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), binder_(binder) {}
    virtual size_opt write(std::span<const std::byte> bytes, std::endian endian = std::endian::native) const override {
        (void)endian;
        if (bytes.size() < sizeof(T))
            return std::nullopt;
        memcpy(&binder_, bytes.data(), sizeof(T));
        return sizeof(T);
    }
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) const override {
        (void)endian;
        if (bytes.size() < sizeof(T))
            return std::nullopt;
        memcpy(bytes.data(), &binder_, sizeof(T));
        return sizeof(T);
    }
};

template <std::integral I> struct VRegBinder<I> : public VRegBase {
    I &binder_;

    constexpr VRegBinder(I &binder, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), binder_(binder) {}
    constexpr VRegBinder(std::endian byte_endian, I &binder, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), binder_(binder) {}
    virtual size_opt write(std::span<const std::byte> bytes, std::endian endian = std::endian::native) override {
        if (bytes.size() < sizeof(I))
            return std::nullopt;
        if (endian == std::endian::native) {
            memcpy(&binder_, bytes.data(), sizeof(I));
            return sizeof(I);
        }
        I tmp;
        memcpy(&tmp, bytes.data(), sizeof(I));
        tmp = vregex::byteswap(tmp);

        binder_ = tmp;
        return sizeof(I);
    }
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) override {
        if (bytes.size() < sizeof(I))
            return std::nullopt;
        if (endian == std::endian::native) {
            memcpy(bytes.data(), &binder_, sizeof(I));
            return sizeof(I);
        }
        const I tmp = vregex::byteswap(tmp);
        memcpy(bytes.data(), &tmp, sizeof(I));
        return sizeof(I);
    }
};

template <class T> struct VRegBinder<const T> : public VRegBase {
    const T &binder_;
    constexpr VRegBinder(const T &binder, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), binder_(binder) {}
    virtual size_opt read(std::span<std::byte> bytes) override {
        if (bytes.size() < sizeof(T))
            return std::nullopt;
        memcpy(bytes.data(), &binder_, sizeof(T));
        return sizeof(T);
    }
};

template <std::integral I> struct VRegBinder<const I> : public VRegBase {
    const I &binder_;

    constexpr VRegBinder(const I &binder, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), binder_(binder) {}

    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) override {
        if (bytes.size() < sizeof(I))
            return std::nullopt;
        if (endian == std::endian::native) {
            memcpy(bytes.data(), &binder_, sizeof(I));
            return sizeof(I);
        }
        const I tmp = vregex::byteswap(tmp);
        memcpy(bytes.data(), &tmp, sizeof(I));
        return sizeof(I);
    }
};

template <class T> static inline auto VRegBinderRO(T &binder, std::string_view name, std::string_view desc = "") {
    return VRegBinder<const T>(binder, name, desc);
}

template <class T> struct VRegConst : public VRegBase {
    const T value_;
    constexpr VRegConst(const T &value, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), value_(value) {}
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) override {
        if (bytes.size() < sizeof(T))
            return std::nullopt;
        memcpy(bytes.data(), &value_, sizeof(T));
        return sizeof(T);
    }
};

template <std::integral T> struct VRegConst<T> : public VRegBase {
    const T value_;
    constexpr VRegConst(const T &value, std::string_view name, std::string_view desc = "")
        : VRegBase(name, desc), value_(value) {}
    virtual size_opt read(std::span<std::byte> bytes, std::endian endian = std::endian::native) override {
        if (bytes.size() < sizeof(T))
            return std::nullopt;
        if (endian == std::endian::native) {
            memcpy(bytes.data(), &value_, sizeof(T));
            return sizeof(T);
        }
        const T tmp = vregex::byteswap(tmp);

        memcpy(bytes.data(), &tmp, sizeof(T));
        return sizeof(T);
    }
};

class VRange : public VMountBase {
    std::vector<std::shared_ptr<VRegBase>> range_; // NOTE: donot resize

public:
    VRange(std::vector<std::shared_ptr<VRegBase>> &&range, std::string_view name, std::string_view desc = "")
        : VMountBase(name, desc), range_(std::move(range)) {}

    virtual size_t size() const override { return range_.size(); }
    virtual size_opt writeAt(addr_t addr, std::span<const std::byte> bytes,
                             std::endian endian = std::endian::native) override {
        if (addr >= range_.size()) {
            return std::nullopt;
        }
        const auto vreg = range_[addr];
        if (!vreg) {
            return std::nullopt;
        }
        return vreg->write(bytes, endian);
    };
    virtual size_opt readAt(addr_t addr, std::span<std::byte> bytes,
                            std::endian endian = std::endian::native) override {
        if (addr >= range_.size()) {
            return std::nullopt;
        }
        const auto vreg = range_[addr];
        if (!vreg) {
            return std::nullopt;
        }
        return vreg->read(bytes, endian);
    };

    auto &at(size_t addr) { return range_.at(addr); }
    const auto &at(size_t addr) const { return range_.at(addr); }
};

class VMap : public VMountBase {
public:
    using pair = std::pair<size_t, std::shared_ptr<VMountBase>>;

private:
    mutable std::vector<pair> ordered_;

public:
    VMap(std::vector<pair> &&ordered, std::string_view name, std::string_view desc)
        : VMountBase(name, desc), ordered_(std::move(ordered)) {
        std::ranges::sort(ordered_, {}, [](const pair &p) { return p.first; });
        // TODO: check overlap
    }

    virtual size_t size() const {
        if (ordered_.empty())
            return 0;
        const auto &back = ordered_.back();
        size_t last_addr = back.first;
        if (auto ptr = back.second; ptr) {
            last_addr += ptr->size();
        }
        return last_addr;
    }

    std::optional<pair> find(size_t addr) const {
        auto end = std::ranges::lower_bound(ordered_, addr + 1, {}, [](const pair &p) { return p.first; });
        auto iter = end - 1;
        if (iter != ordered_.begin()) {
            const auto &[offset, mount] = *iter;
            const size_t size = mount ? mount->size() : 0;
            if (offset <= addr && addr < offset + size) {
                return *iter;
            }
        }
        return std::nullopt;
    }

    virtual size_opt writeAt(size_t addr, std::span<const std::byte> bytes) const {
        if (auto opt = find(addr)) {
            const auto &[offset, mount] = *opt;
            return mount->writeAt(addr - offset, bytes);
        } else {
            return std::nullopt;
        }
    };
    virtual size_opt readAt(size_t addr, std::span<std::byte> bytes) const {
        if (auto opt = find(addr)) {
            const auto &[offset, mount] = *opt;
            return mount->readAt(addr - offset, bytes);
        } else {
            return std::nullopt;
        }
    };
};

}; // namespace vreg::impl