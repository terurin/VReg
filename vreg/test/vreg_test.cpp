#include <gtest/gtest.h>
#include <vector>
#include <vreg.hpp>
using namespace vreg;
namespace vreg_test {
TEST(VReg, VReg) {
    bool w = false, r = false;
    VReg wr(
        [&w](std::span<const std::byte> bytes, std::endian endian) -> size_opt {
            (void)bytes;
            (void)endian;

            w = true;
            return 1;
        },
        [&r](std::span<std::byte> bytes, std::endian endian) -> size_opt {
            (void)bytes;
            (void)endian;
            r = true;
            return 1;
        },
        "wr", "test");

    // VMountBase
    EXPECT_EQ(wr.name_, "wr");
    EXPECT_EQ(wr.desc_, "test");
    // action test
    std::vector<std::byte> bytes(10);
    EXPECT_EQ(wr.write(bytes), 1);
    EXPECT_EQ(w, true);
    EXPECT_EQ(wr.read(bytes), 1);
    EXPECT_EQ(r, true);
}

TEST(VReg, VRegWO) {
    bool w = false;
    auto wo = VRegWO(
        [&w](std::span<const std::byte> bytes, std::endian endian) -> size_opt {
            (void)bytes;
            (void)endian;
            w = true;
            return 1;
        },
        "wo", "test");

    // VMountBase
    EXPECT_EQ(wo.name_, "wo");
    EXPECT_EQ(wo.desc_, "test");
    // action test
    std::vector<std::byte> bytes(10);
    EXPECT_EQ(wo.write(bytes), 1);
    EXPECT_EQ(w, true);
    EXPECT_EQ(wo.read(bytes), std::nullopt);
}

TEST(VReg, VRegRO) {
    bool r = false;
    auto ro = VRegRO(
        [&r](std::span<std::byte> bytes, std::endian endian) -> size_opt {
            (void)bytes;
            (void)endian;
            r = true;
            return 1;
        },
        "ro", "test");

    // VMountBase
    EXPECT_EQ(ro.name_, "ro");
    EXPECT_EQ(ro.desc_, "test");
    // action test
    std::vector<std::byte> bytes(10);
    EXPECT_EQ(ro.write(bytes), std::nullopt);
    EXPECT_EQ(ro.read(bytes), 1);
    EXPECT_EQ(r, true);
}

TEST(VRegBinder, wr) {
    uint32_t value = 0;
    VRegBinder wr(value, "wr", "test");
    // VMountBase
    EXPECT_EQ(wr.name_, "wr");
    EXPECT_EQ(wr.desc_, "test");
    // action test
    uint32_t v = 1;
    std::byte bytes[sizeof(v)];
    memcpy(bytes, &v, sizeof(bytes));
    EXPECT_EQ(wr.write(bytes), sizeof(uint32_t));
    memset(bytes, 0, sizeof(bytes));
    EXPECT_EQ(wr.read(bytes), sizeof(uint32_t));
    uint32_t w;
    memcpy(&w, bytes, sizeof(w));
    EXPECT_EQ(w, 1);
}

TEST(VRegBinder, ro) {
    const uint32_t value = 1;
    VRegBinder ro(value, "ro", "test");
    // VMountBase
    EXPECT_EQ(ro.name_, "ro");
    EXPECT_EQ(ro.desc_, "test");
    // action test
    uint32_t v = 0;
    std::byte bytes[sizeof(v)];
    memset(bytes, 0, sizeof(bytes));
    EXPECT_EQ(ro.read(bytes), sizeof(uint32_t));
    memcpy(&v, bytes, sizeof(v));
    EXPECT_EQ(v, 1);
}

TEST(VRegConst, ro) {
    const uint32_t value = 1;
    VRegConst c(value, "const", "test");
    // VMountBase
    EXPECT_EQ(c.name_, "const");
    EXPECT_EQ(c.desc_, "test");
    // action test
    uint32_t v = 0;
    std::byte bytes[sizeof(v)];
    memset(bytes, 0, sizeof(bytes));
    EXPECT_EQ(c.read(bytes), sizeof(uint32_t));
    memcpy(&v, bytes, sizeof(v));
    EXPECT_EQ(v, 1);
}

} // namespace vreg_test

namespace vrange_test {
TEST(VRange, VRange) {
    VRangeBuilder rb("range", "desc");
    int a = 0, b = 1;
    rb.add(VRegBuilder("a").buildBinder(a));
    rb.add(VRegBuilder("b").buildBinder(b));
    rb.add(VRegBuilder("c").buildReserved());
    VRange r = rb.build();

    // check prop
    EXPECT_EQ(r.name_, "range");
    EXPECT_EQ(r.desc_, "desc");
    EXPECT_EQ(r.size(), 3);
    EXPECT_EQ(r.mask(), 3);
    EXPECT_EQ(r.maskWidth(), 2);

    // write test in range
    std::byte buf[sizeof(a)];
    int aa = 2;
    memcpy(buf, &aa, sizeof(buf));
    EXPECT_EQ(r.writeAt(0, buf), sizeof(int));
    EXPECT_EQ(a, aa);

    // read test
    memset(buf, 0, sizeof(buf));
    EXPECT_EQ(r.readAt(1, buf), sizeof(int));
    int bb;
    memcpy(&bb, buf, sizeof(bb));
    EXPECT_EQ(b, bb);

    // out of range
    EXPECT_EQ(r.readAt(4, buf), std::nullopt);
    EXPECT_EQ(r.writeAt(4, buf), std::nullopt);
}
} // namespace vrange_test

namespace VMap {
    
};