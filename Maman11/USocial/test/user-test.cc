#include <gtest/gtest.h>
#include <stdexcept>
#include "Maman11/USocial/USocial.h"
#include "Maman11/USocial/User.h"
#include "Maman11/USocial/Photo.h"
#include "Maman11/USocial/Message.h"
#include "Maman11/USocial/USocialExceptions.h"

class UserTests : public ::testing::Test {
protected:
    void SetUp() override {
        user = us.registerUser("user1");
    }

    USocial us;
    User* user;
};

class TwoUserTests: public UserTests {
protected:
    void SetUp() override {
        UserTests::SetUp();
        user2 = us.registerUser("user2");
    }

    User* user2;
};

TEST_F(UserTests, TestUserCreated) {
    EXPECT_EQ(user->getName(), "user1");
    EXPECT_EQ(user->getId(), 1);
}

// ------------- Friend tests ------------------
TEST_F(UserTests, TestAddFriendNullptrFails) {
    EXPECT_THROW({
        user->addFriend(nullptr);
    }, std::invalid_argument);
}

TEST_F(TwoUserTests, TestAddFriendTwiceFails) {
    user->addFriend(user2);

    EXPECT_THROW({
        user->addFriend(user2);
    }, std::invalid_argument);
}

TEST_F(UserTests, TestRemoveFriendNullptrFails) {
    EXPECT_THROW({
        user->removeFriend(nullptr);
    }, std::invalid_argument);
}

TEST_F(TwoUserTests, TestFriendRemoved) {
    // We remove and add the same friend and make sure it doesn't throw
    user->addFriend(user2);
    user->removeFriend(user2);
    user->addFriend(user2);
}

// ---------------- PostTests -----------------
TEST_F(UserTests, TestPostNoMedia) {
    user->post("mypost");
    EXPECT_EQ(user->getPosts().size(), 1);
    Post* p = *user->getPosts().begin();
    EXPECT_EQ(p->getText(), "mypost");
}

TEST_F(UserTests, TestPostWithMedia) {
    user->post("mypost", new Photo());
    EXPECT_EQ(user->getPosts().size(), 1);
    Post* p = *user->getPosts().begin();
    EXPECT_EQ(p->getText(), "mypost");

    testing::internal::CaptureStdout();
    p->getMedia()->display();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "image\n");
}


// ---------------- Message Tests ----------------
TEST_F(UserTests, TestNullPtrRecieveMessage) {
     EXPECT_THROW({
        user->receiveMessage(nullptr);
    }, std::invalid_argument);
}

TEST_F(UserTests, TestReceiveMessage) {
    user->receiveMessage(new Message("mymessage"));
    testing::internal::CaptureStdout();
    user->viewReceivedMessages();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "viewReceivedMessages: " + std::to_string(user->getId()) + "\nmymessage\n");
}

TEST_F(UserTests, TestSendMessageNullMessageThrows) {
    EXPECT_THROW({
        user->sendMessage(user, nullptr);
    }, std::invalid_argument);
}

TEST_F(UserTests, TestSendMessageNullUser) {
    Message* message = new Message("mymessage");
    EXPECT_THROW({
        user->sendMessage(nullptr, message);
    }, std::invalid_argument);
}

TEST_F(UserTests, TestSendMessageSameUser) {
    Message* message = new Message("mymessage");
    EXPECT_THROW({
        user->sendMessage(user, message);
    }, std::invalid_argument);
}

TEST_F(TwoUserTests, TestSendMessageDifferentUser) {
    Message* message = new Message("mymessageto2");
    user->addFriend(user2);
    user->sendMessage(user2, message);

    testing::internal::CaptureStdout();
    user2->viewReceivedMessages();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "viewReceivedMessages: " + std::to_string(user2->getId()) + "\nmymessageto2\n");
}

TEST_F(TwoUserTests, TestSendToANonFriend) {
    Message* message= new Message("mymessage");
    EXPECT_THROW({
        user->sendMessage(user2, message);
    }, UserIsNotAFriendException);
}

TEST_F(UserTests, TestBusinessUserSendToNonFriend) {
    Message* message = new Message("mymessagefrombu");
    User* bu = us.registerUser("bu", true);
    bu->sendMessage(user, message);

    testing::internal::CaptureStdout();
    user->viewReceivedMessages();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "viewReceivedMessages: " + std::to_string(user->getId()) + "\nmymessagefrombu\n");
}
