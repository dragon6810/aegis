#include "pch.h"
#include "Quaternion.h"
#include "Mat3x4.h"

bool approxequal(float a, float b)
{
    return (a > (b - 0.001F)) && (a < (b + 0.001F));
}

TEST(TestCaseName, TestName)
{
    Quaternion q;
    Mat3x4 m;

    q = Quaternion::FromAngle(new float[3] {90.0F * 0.0174533F, 0.0F * 0.0174533F, 0.0F * 0.0174533F});
    EXPECT_NEAR(q.q[0], 0.7071068F, 0.001F);
    EXPECT_NEAR(q.q[1], 0.0F, 0.001F);
    EXPECT_NEAR(q.q[2], 0.0F, 0.001F);
    EXPECT_NEAR(q.q[3], 0.7071068F, 0.001F);

    q = Quaternion::FromAngle(new float[3] {90.0F * 0.0174533F, 45.0F * 0.0174533F, 0.0F * 0.0174533F});
    EXPECT_NEAR(q.q[0], 0.6532815F, 0.001F);
    EXPECT_NEAR(q.q[1], 0.2705981F, 0.001F);
    EXPECT_NEAR(q.q[2], -0.2705981F, 0.001F);
    EXPECT_NEAR(q.q[3], 0.6532815F, 0.001F);
}