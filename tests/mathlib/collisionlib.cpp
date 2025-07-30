#include <gtest/gtest.h>

#include <mathlib.h>

bool testhitresult(Mathlib::hitresult_t out, Mathlib::hitresult_t expect)
{
    const float epsilon = 0.01;

    if(out.hit != expect.hit)
        return false;

    if(!out.hit)
        return true;

    if((expect.pos - out.pos).norm() > epsilon)
        return false;

    if((expect.normal - out.normal).norm() > epsilon)
        return false;

    return true;
}

TEST(RayCuboidTests, MissTests)
{
    Mathlib::hitresult_t out, expect;
    Eigen::Vector3f o, r, cube;
    float radius;

    o = Eigen::Vector3f(16, 0, 0);
    r = Eigen::Vector3f(1, 0, 0);
    radius = 1.0;
    cube = Eigen::Vector3f(0.0, 0.0, 0.0);
    expect = { false, Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 0, 0) };
    out = Mathlib::RayCuboid(o, r, cube, radius);
    EXPECT_TRUE(testhitresult(out, expect));
}

TEST(RayCuboidTests, NominalTests)
{
    Mathlib::hitresult_t out, expect;
    Eigen::Vector3f o, r, cube;
    float radius;

    o = Eigen::Vector3f(16, 0, 0);
    r = Eigen::Vector3f(-1, 0, 0);
    cube = Eigen::Vector3f(0.0, 0.0, 0.0);
    radius = 1.0;
    expect = { true, Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(1, 0, 0) };
    out = Mathlib::RayCuboid(o, r, cube, radius);
    EXPECT_TRUE(testhitresult(out, expect));
}