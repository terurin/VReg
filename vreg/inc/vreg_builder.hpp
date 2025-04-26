#pragma once
#include "vreg_impl.hpp"

namespace vreg::builder {
using base::VRegBase, base::VRegBasePtr;
using base::writer, base::reader;
using impl::VMap, impl::VRange;
using impl::VReg, impl::VRegRO, impl::VRegWO, impl::VRegReserved;
using impl::VRegBinder, impl::VRegBinderRO;
using impl::VRegConst;
using std::move;
class VRegBuilder {
    std::string_view name_, desc_;

public:
    VRegBuilder(std::string_view name, std::string_view desc = "") : name_(name), desc_(desc) {}
    VRegBuilder &setName(std::string_view name) { return name_ = name, *this; }
    VRegBuilder &setDesc(std::string_view desc) { return desc_ = desc, *this; }

    // VReg
    template <writer W, reader R> VRegBasePtr buildRW(W &&writer, R &&reader) {
        return std::make_shared<VReg<W, R>>(std::move(writer), std::move(reader), name_, desc_);
    }
    template <writer W> VRegBasePtr buildWO(W &&writer) {
        auto wo = VRegWO(move(writer), name_, desc_);
        return std::make_shared<decltype(wo)>(move(wo));
    }
    template <reader R> VRegBasePtr buildRO(R &&reader) {
        auto ro = VRegRO(move(reader), name_, desc_);
        return std::make_shared<decltype(ro)>(move(ro));
    }
    VRegBasePtr buildReserved() {
        auto reg = VRegReserved(name_, desc_);
        return std::make_shared<decltype(reg)>(move(reg));
    }

    // VRegBinder
    template <class T>VRegBasePtr buildBinder(T& binder){
        return std::make_shared<VRegBinder<T>>(binder, name_, desc_);
    }
    template <class T>VRegBasePtr buildBinderRO(T& binder){
        auto vreg = VRegBinderRO(binder, name_, desc_);
        return std::make_shared<VRegBinder<T>>(move(vreg));
    }
    // VRegConst
    template <class T>VRegBasePtr buildConst(const T&value){
        return std::make_shared<VRegConst<T>>(value, name_, desc_);
    }
};

class VRangeBuilder {
    std::string_view name_, desc_;
    std::vector<std::shared_ptr<VRegBase>> range_;

public:
    VRangeBuilder(std::string_view name, std::string_view desc = "") : name_(name), desc_(desc) {}
    VRangeBuilder &setName(std::string_view name) { return name_ = name, *this; }
    VRangeBuilder &setDesc(std::string_view desc) { return desc_ = desc, *this; }

    // VRegBase(General)
    VRangeBuilder &add(std::shared_ptr<VRegBase>&& vreg) { return range_.emplace_back(vreg), *this; }
    VRange build() { return VRange(std::move(range_), name_, desc_); }
};

} // namespace vreg::builder