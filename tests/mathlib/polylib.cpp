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

TEST(EquivalentPolysTests, ZeroOffsetTests)
{
    Mathlib::Poly<2> a, b;

    a = 
    {
        { 0, 0, },
        { 1, 0, },
        { 1, 1, },
        { 0, 1, },
    };
    b = a;
    EXPECT_EQ(Mathlib::EquivalentPolys(a, b), true);

    b[1] = { 1, -1, };
    EXPECT_EQ(Mathlib::EquivalentPolys(a, b), false);
}

TEST(EquivalentPolysTests, OffsetTests)
{
    Mathlib::Poly<2> a, b;

    a = 
    {
        { 0, 0, },
        { 1, 0, },
        { 1, 1, },
        { 0, 1, },
    };
    b = 
    {
        { 1, 0, },
        { 1, 1, },
        { 0, 1, },
        { 0, 0, },
    };
    EXPECT_EQ(Mathlib::EquivalentPolys(a, b), true);

    b[2] = { 1, -1, };
    EXPECT_EQ(Mathlib::EquivalentPolys(a, b), false);
}

TEST(PolySideTests, DistinctTests)
{
    Mathlib::Poly<2> in;
    Eigen::Vector2f n;
    float d;
    Mathlib::planeside_e expect;

    in = 
    {
        { -1, -1, },
        { 1, -1, },
        { 1, 1 },
    };
    n = { 1, 0, };
    d = 2;
    expect = Mathlib::SIDE_BACK;
    EXPECT_EQ(Mathlib::PolySide(in, n, d), expect);

    in = 
    {
        { -1, -1, },
        { 1, -1, },
        { 1, 1 },
    };
    n = { -1, -1, };
    n.normalize();
    d = -3;
    expect = Mathlib::SIDE_FRONT;
    EXPECT_EQ(Mathlib::PolySide(in, n, d), expect);
}

TEST(PolySideTests, CrossTests)
{
    Mathlib::Poly<2> in;
    Eigen::Vector2f n;
    float d;
    Mathlib::planeside_e expect;

    in = 
    {
        { -1, -1, },
        { 1, -1, },
        { 1, 1 },
    };
    n = { 1, 0, };
    d = 0.5;
    expect = Mathlib::SIDE_CROSS;
    EXPECT_EQ(Mathlib::PolySide(in, n, d), expect);
}

TEST(PolySideTests, OnTests)
{
    Mathlib::Poly<2> in;
    Eigen::Vector2f n;
    float d;
    Mathlib::planeside_e expect;

    in = 
    {
        { -1, -1, },
        { 1, -1, },
    };
    n = { 0, -1, };
    d = 1;
    expect = Mathlib::SIDE_ON;
    EXPECT_EQ(Mathlib::PolySide(in, n, d), expect);
}

TEST(FromPlaneTests, AxialTests)
{
    Mathlib::Poly<3> out, expect;
    Eigen::Vector3f n;
    float d;

    n = Eigen::Vector3f(1, 0, 0);
    d = 0;
    expect = 
    {
        { 0, -1, -1, },
        { 0, 1, -1, },
        { 0, 1, 1 },
        { 0, -1, 1},
    };
    out = Mathlib::FromPlane(n, d, 1);
    EXPECT_TRUE(Mathlib::EquivalentPolys(out, expect));
}