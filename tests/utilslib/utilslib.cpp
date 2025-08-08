#include <gtest/gtest.h>

#include <utilslib.h>

TEST(StripExtensionsTests, EmptyTests)
{
    EXPECT_EQ("", Utilslib::StripExtension(""));
}

TEST(StripExtensionTests, NoExtensionTests)
{
    EXPECT_EQ("filea", Utilslib::StripExtension("filea"));
    EXPECT_EQ("42", Utilslib::StripExtension("42"));
    EXPECT_EQ("42withletters", Utilslib::StripExtension("42withletters"));
    EXPECT_EQ("letterswith42", Utilslib::StripExtension("letterswith42"));
    EXPECT_EQ("path/to/42", Utilslib::StripExtension("path/to/42"));
    EXPECT_EQ("42/to/path", Utilslib::StripExtension("42/to/path"));
    EXPECT_EQ("/abs/file/path", Utilslib::StripExtension("/abs/file/path"));
}

TEST(StripExtensionTests, NominalTests)
{
    EXPECT_EQ("iamavideo", Utilslib::StripExtension("iamavideo.mp4"));
    EXPECT_EQ("/i/am/a/path/to/a/video", Utilslib::StripExtension("/i/am/a/path/to/a/video.mp4"));
}

TEST(AddExtensionTests, EmptyTests)
{
    EXPECT_EQ(".", Utilslib::AddExtension("", ""));
    EXPECT_EQ(".ext", Utilslib::AddExtension("", "ext"));
    EXPECT_EQ("file.", Utilslib::AddExtension("file", ""));
}

TEST(AddExtensionTests, AlreadyExtensionTests)
{
    EXPECT_EQ("file.mp4.zip", Utilslib::AddExtension("file.mp4", "zip"));
    EXPECT_EQ("file.mp4.zip", Utilslib::AddExtension("file", "mp4.zip"));
}

TEST(AddExtensionTests, NominalTests)
{
    EXPECT_EQ("iwanttobeavideo.mp4", Utilslib::AddExtension("iwanttobeavideo", "mp4"));
}

TEST(DefaultExtensionTests, NoExtensionTests)
{
    EXPECT_EQ("ireallywanttobeasong.mp3", Utilslib::DefaultExtension("ireallywanttobeasong", "mp3"));
}

TEST(DefaultExtensionTests, AlreadyExtensionTests)
{
    EXPECT_EQ("iwanttostayavideo.mp4", Utilslib::DefaultExtension("iwanttostayavideo.mp4", "mp3"));
}