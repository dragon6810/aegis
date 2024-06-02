#include "pch.h"
#include "Quaternion.h"
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
    Quaternion q;
    Mat3x4 m, r, t;

    q = Quaternion::FromAngle(new float[3] {90.0F * 0.0174533F, 0.0F * 0.0174533F, 0.0F * 0.0174533F});
    EXPECT_NEAR(q.q[0], 0.71F, 0.01F);
    EXPECT_NEAR(q.q[1], 0.71F, 0.01F);
    EXPECT_NEAR(q.q[2], 0.0F, 0.01F);
    EXPECT_NEAR(q.q[3], 0.0F, 0.01F);

    q = Quaternion::FromAngle(new float[3] {90.0F * 0.0174533F, 45.0F * 0.0174533F, 0.0F * 0.0174533F});
    EXPECT_NEAR(q.q[0], 0.65F, 0.01F);
    EXPECT_NEAR(q.q[1], 0.65F, 0.01F);
    EXPECT_NEAR(q.q[2], 0.27F, 0.01F);
    EXPECT_NEAR(q.q[3], 0.27F, 0.01F);

    m.val[0][0] =  0.71F; m.val[0][1] =   0.0F; m.val[0][2] =  0.71F; m.val[0][3] = 0.0F;
    m.val[1][0] =  0.71F; m.val[1][1] =   0.0F; m.val[1][2] = -0.71F; m.val[1][3] = 0.0F;
    m.val[2][0] =   0.0F; m.val[2][1] =   1.0F; m.val[2][2] =   0.0F; m.val[2][3] = 0.0F;

    compMats(q.toMat(), m);

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