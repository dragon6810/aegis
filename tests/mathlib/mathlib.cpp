#include <gtest/gtest.h>

#include <mathlib.h>

TEST(DEG2RADTests, NominalTests)
{
    EXPECT_NEAR(DEG2RAD(90), 1.5708, 0.01);
    EXPECT_NEAR(DEG2RAD(23), 0.401426, 0.01);
    EXPECT_NEAR(DEG2RAD(198), 3.45575, 0.01);
    EXPECT_NEAR(DEG2RAD(285), 4.97419, 0.01);
}

TEST(DEG2RADTests, OutOfBoundsTests)
{
    EXPECT_NEAR(DEG2RAD(-35), -0.610865, 0.01);
    EXPECT_NEAR(DEG2RAD(420), 7.33038, 0.01);
}

TEST(RAD2DEGTests, NominalTests)
{
    EXPECT_NEAR(RAD2DEG(1.5708), 90, 0.01);
    EXPECT_NEAR(RAD2DEG(0.401426), 23, 0.01);
    EXPECT_NEAR(RAD2DEG(3.45575), 198, 0.01);
    EXPECT_NEAR(RAD2DEG(4.97419), 285, 0.01);
}

TEST(RAD2DEGTests, OutOfBoundsTests)
{
    EXPECT_NEAR(RAD2DEG( -0.610865), -35, 0.01);
    EXPECT_NEAR(RAD2DEG(7.33038), 420, 0.01);
}

TEST(LERPTests, NominalTests)
{
    EXPECT_NEAR(LERP(-5, -10, 0.5), -7.5, 0.01);
    EXPECT_NEAR(LERP(0, 1, 2), 2, 0.01);
    EXPECT_NEAR(LERP(12, 16, -1), 8, 0.01);
    EXPECT_NEAR(LERP(8, 16, 0), 8, 0.01);
    EXPECT_NEAR(LERP(32, 64, 1), 64, 0.01);
}

TEST(FromEulerTests, NominalTests)
{
    EXPECT_NEAR
    (
        Mathlib::FromEuler(Eigen::Vector3f(0, 0, 0)).angularDistance
        (Eigen::Quaternionf(1, 0, 0, 0)),
        0, 0.01
    );
    EXPECT_NEAR
    (
        Mathlib::FromEuler(Eigen::Vector3f(M_PI_2, 0, 0)).angularDistance
        (Eigen::Quaternionf(M_SQRT1_2, M_SQRT1_2, 0, 0)),
        0, 0.01
    );
    EXPECT_NEAR
    (
        Mathlib::FromEuler(Eigen::Vector3f(M_PI_2, 0, M_PI_4)).angularDistance
        (Eigen::Quaternionf(0.6532815, 0.6532815, 0.2705981, 0.2705981)),
        0, 0.01
    );
    EXPECT_NEAR
    (
        Mathlib::FromEuler(Eigen::Vector3f(M_PI_2, M_PI, M_PI_4)).angularDistance
        (Eigen::Quaternionf(0.2705981, -0.2705981, 0.6532815, -0.6532815)),
        0, 0.01
    );
}