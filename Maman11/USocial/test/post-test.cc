#include <gtest/gtest.h>

#include "Maman11/USocial/Post.h"
#include "Maman11/USocial/Photo.h"

TEST(PostTests, TestPostNoMedia) {
    Post p("mypost");
    EXPECT_EQ(p.getText(), "mypost");
    EXPECT_EQ(p.getMedia(), nullptr);
}

TEST(PostTests, TestPostWithMedia) {
    Photo* photo = new Photo();
    Post p("mypost", photo);
    EXPECT_EQ(p.getText(), "mypost");
    EXPECT_EQ(p.getMedia(), photo);
}

TEST(PostTests, TestPrintPostNoMedia) {
    Post p("mypost", nullptr);
    testing::internal::CaptureStdout();
    p.printPost();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ("============\nmypost\n============\n", output);
}
