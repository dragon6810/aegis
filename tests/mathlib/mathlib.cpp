#include <gtest/gtest.h>

#include <mathlib.h>

TEST(DEG2RADTests, NominalTests)
{
    EXPECT_EQ(DEG2RAD(90), M_PI_2);
}