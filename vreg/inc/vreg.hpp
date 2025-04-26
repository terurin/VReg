#pragma once

#include "vreg_base.hpp"
#include "vreg_builder.hpp"
#include "vreg_impl.hpp"
namespace vreg {

// shared
using base::size_opt, base::addr_t;
using base::VRegBase;
using base::writer, base::reader;

// vreg
using impl::VReg, impl::VRegWO, impl::VRegRO,impl::VRegReserved;
using impl::VRegBinder;
using impl::VRegConst;

//

// vrange
using impl::VMap;
using impl::VRange;

// builders
using builder::VRangeBuilder, builder::VRegBuilder;

} // namespace vreg