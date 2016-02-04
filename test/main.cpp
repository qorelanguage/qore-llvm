#include "gmock/gmock.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
