#include <cstdint>
#include <gtest/gtest.h>
#include <vregex.hpp>
using namespace vregex;

TEST(byteswap, byteswap) {
    EXPECT_EQ(byteswap<bool>(true), true);
    EXPECT_EQ(byteswap<uint8_t>(0x81), 0x81);
    EXPECT_EQ(byteswap<uint16_t>(0x1122), 0x2211);
    EXPECT_EQ(byteswap<uint32_t>(0x11223344), 0x44332211);
    EXPECT_EQ(byteswap<uint64_t>(0x1122334455667788UL), 0x8877665544332211UL);
}

TEST(varchar, adjust_size) {

    const varchar<7> eight("12345678");
    const varchar<7> seven("1234567");
    EXPECT_EQ(eight, seven);
    EXPECT_EQ((std::string_view)eight, "1234567");
    EXPECT_EQ(eight.size(), 7);
    EXPECT_EQ(eight.max_size(), 7);

    const varchar<3> three(seven);
    EXPECT_EQ(three, "123");
    EXPECT_EQ(three.size(), 3);
    EXPECT_EQ(three.max_size(), 3);
}

TEST(varchar, iterator) {
    varchar<7> eight("1234567");
    EXPECT_EQ(*eight.cbegin(), '1');
    EXPECT_EQ(*(eight.cend() - 1), '7');
    EXPECT_EQ(*eight.cend(), '\0');
}

TEST(varchar, modify) {
    varchar<7> x = "1";
    x += "2";
    const varchar<7> y = "12";
    EXPECT_TRUE(x== y);
}
