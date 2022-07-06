#include <gtest/gtest.h>
#include "Maman11/USocial/USocial.h"
#include "Maman11/USocial/USocialExceptions.h"
#include "Maman11/USocial/User.h"

class USocialTests : public ::testing::Test {
protected:
    USocial us;
};

TEST_F(USocialTests, TestRegisterUser) {
    User* user = us.registerUser("user1");
    EXPECT_EQ(user->getName(), "user1");
}

TEST_F(USocialTests, TestGetInvalidUserById) {
    EXPECT_THROW({
        us.getUserById(1337);
    }, InvalidUserID);
}

TEST_F(USocialTests, TestGetUserById) {
    User* u = us.registerUser("myuser");
    EXPECT_EQ(us.getUserById(u->getId()), u);
}
