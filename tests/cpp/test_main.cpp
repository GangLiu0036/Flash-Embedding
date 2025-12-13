#include <gtest/gtest.h>

#include "flash_embed.h"

// Basic test to verify GTest integration
TEST(FlashEmbedTest, Addition) {
    EXPECT_EQ(flash_embed::add(1, 2), 3);
    EXPECT_EQ(flash_embed::add(-1, 1), 0);
    EXPECT_EQ(flash_embed::add(10, 20), 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
