#include <cstdint>
#include <gtest/gtest.h>
#include <vregex.hpp>
using namespace vregex;

TEST(vregex, byteswap) {
    EXPECT_EQ(byteswap<bool>(true), true);
    EXPECT_EQ(byteswap<uint8_t>(0x81), 0x81);
    EXPECT_EQ(byteswap<uint16_t>(0x1122), 0x2211);
    EXPECT_EQ(byteswap<uint32_t>(0x11223344), 0x44332211);
    EXPECT_EQ(byteswap<uint64_t>(0x1122334455667788UL), 0x8877665544332211UL);
}
