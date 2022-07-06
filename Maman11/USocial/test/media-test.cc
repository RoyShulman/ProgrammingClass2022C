#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <tuple>

#include "Maman11/USocial/Photo.h"
#include "Maman11/USocial/Audio.h"
#include "Maman11/USocial/Video.h"

using std::string;
using std::tuple;


class MediaTest : public testing::TestWithParam<tuple<const string, const Media*>> {
    void SetUp() override {
        expectedOutput = std::get<0>(GetParam());
        m = std::get<1>(GetParam());

        testing::internal::CaptureStdout();
    }

public:
    string expectedOutput;
    const Media* m;
};

TEST_P(MediaTest, TestDisplay) {
    m->display();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, expectedOutput);
    delete m;
};

static const tuple<const string, const Media*> mediaTestValues[] = {
    std::make_tuple(string("image\n"), new Photo()),
    std::make_tuple(string("audio\n"), new Audio()),
    std::make_tuple(string("video\n"), new Video()),
};

INSTANTIATE_TEST_SUITE_P(MediaTestparams,
                         MediaTest,
                         testing::ValuesIn(mediaTestValues));