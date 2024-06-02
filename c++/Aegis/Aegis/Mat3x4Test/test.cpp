#include "pch.h"
#include "Mat3x4.h"

bool approxequal(float a, float b)
{
    return (a > (b - 0.01F)) && (a < (b + 0.01F));
}

bool compMats(const Mat3x4& a, const Mat3x4& b)
{
    bool failed = false;

    EXPECT_NEAR(a.val[0][0], b.val[0][0], 0.01F);
    if (!approxequal(a.val[0][0], b.val[0][0]))
        failed = true;

    EXPECT_NEAR(a.val[1][0], b.val[1][0], 0.01F);
    if (!approxequal(a.val[1][0], b.val[1][0]))
        failed = true;

    EXPECT_NEAR(a.val[2][0], b.val[2][0], 0.01F);
    if (!approxequal(a.val[2][0], b.val[2][0]))
        failed = true;

    EXPECT_NEAR(a.val[0][1], b.val[0][1], 0.01F);
    if (!approxequal(a.val[0][1], b.val[0][1]))
        failed = true;

    EXPECT_NEAR(a.val[1][1], b.val[1][1], 0.01F);
    if (!approxequal(a.val[1][1], b.val[1][1]))
        failed = true;

    EXPECT_NEAR(a.val[2][1], b.val[2][1], 0.01F);
    if (!approxequal(a.val[2][1], b.val[2][1]))
        failed = true;

    EXPECT_NEAR(a.val[0][2], b.val[0][2], 0.01F);
    if (!approxequal(a.val[0][2], b.val[0][2]))
        failed = true;

    EXPECT_NEAR(a.val[1][2], b.val[1][2], 0.01F);
    if (!approxequal(a.val[1][2], b.val[1][2]))
        failed = true;

    EXPECT_NEAR(a.val[2][2], b.val[2][2], 0.01F);
    if (!approxequal(a.val[2][2], b.val[2][2]))
        failed = true;

    EXPECT_NEAR(a.val[0][3], b.val[0][3], 0.01F);
    if (!approxequal(a.val[0][3], b.val[0][3]))
        failed = true;

    EXPECT_NEAR(a.val[1][3], b.val[1][3], 0.01F);
    if (!approxequal(a.val[1][3], b.val[1][3]))
        failed = true;

    EXPECT_NEAR(a.val[2][3], b.val[2][3], 0.01F);
    if (!approxequal(a.val[2][3], b.val[2][3]))
        failed = true;

    return !failed;
}

TEST(TestCaseName, TestName)
{
    Mat3x4 m, t, r;
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);

    m.val[0][0] = 0.71F; m.val[0][1] = 0.0F; m.val[0][2] = 0.71F; m.val[0][3] = 0.0F;
    m.val[1][0] = 0.71F; m.val[1][1] = 0.0F; m.val[1][2] = -0.71F; m.val[1][3] = 0.0F;
    m.val[2][0] = 0.0F; m.val[2][1] = 1.0F; m.val[2][2] = 0.0F; m.val[2][3] = 0.0F;

    t = Mat3x4::getIdentity();
    t.val[0][3] = 3.0F;
    t.val[1][3] = 2.0F;
    t.val[2][3] = 4.0F;
    for (int ro = 0; ro < 3; ro++)
    {
        for (int c = 0; c < 4; c++)
            r.val[ro][c] = m.val[ro][c];
    }
    m.val[0][3] = 3.0F;
    m.val[1][3] = 2.0F;
    m.val[2][3] = 4.0F;

    compMats(t * r, m);
}