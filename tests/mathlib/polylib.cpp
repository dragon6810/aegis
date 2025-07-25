#include <gtest/gtest.h>

#include <mathlib.h>

TEST(ClipPolyTests, TotalClipTests)
{
    Mathlib::Poly<2> in, out, expect;
    Eigen::Vector2f n;
    float d;

    in = 
    { 
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
    };
    expect = in;
    n = { 1, 0 };
    d = 1;
    out = Mathlib::ClipPoly(in, n, d, Mathlib::SIDE_BACK);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));

    n = { 0, -1 };
    d = -1;
    out = Mathlib::ClipPoly(in, n, d, Mathlib::SIDE_FRONT);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));

    expect.clear();
    out = Mathlib::ClipPoly(in, n, d, Mathlib::SIDE_BACK);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));
}

TEST(ClipPolyTests, NominalTests)
{
    Mathlib::Poly<2> in, out, expect;
    Eigen::Vector2f n;
    float d;

    in = 
    { 
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
    };
    expect =
    {
        { 0, 0 },
        { 0.5, 0 },
        { 0.5, 1 },
        { 0, 1 },
    };
    n = { 1, 0 };
    d = 0.5;
    out = Mathlib::ClipPoly(in, n, d, Mathlib::SIDE_BACK);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));

    expect =
    {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
    };

    n = { 1, 1 };
    n.normalize();
    d = n.dot(Eigen::Vector2f(0.5, 0.5));
    out = Mathlib::ClipPoly(in, n, d, Mathlib::SIDE_BACK);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));
}