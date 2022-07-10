#include <gtest/gtest.h>
#include "credit.h"

static const int MIN_CREDIT = -100;
static const int MAX_CREDIT = 1000;
static const int BOUND = 750;

TEST(CreditTests, TestBrokenFunction) {
    for (int i = MIN_CREDIT; i <= MAX_CREDIT; i++) {
        bool is_entitled = is_entitled_for_promotional_gift(i);
        if (i < 0 || i >= BOUND) {
            EXPECT_TRUE(is_entitled);
        } else {
            EXPECT_FALSE(is_entitled);
        }
    }
}

TEST(CreditTests, TestFixedFunction) {
    for (int i = MIN_CREDIT; i <= MAX_CREDIT; i++) {
        bool is_entitled = is_entitled_for_promotional_gift_fixed(i);
        if (i >= BOUND) {
            EXPECT_TRUE(is_entitled);
        } else {
            EXPECT_FALSE(is_entitled);
        }
    }
}
