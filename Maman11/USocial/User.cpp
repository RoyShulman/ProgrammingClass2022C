#include "User.h"
#include <stdexcept>
#include <iostream>
#include "USocialExceptions.h"

User::~User() {
    for (auto &post : posts_) {
        delete post;
    }

    for (auto &message: receivedMsgs_) {
        delete message;
    }
}

User::User(const User& other) : us_(other.us_), id_(other.id_), name_(other.name_) {
    for (auto friendID: other.friends) {
        friends.push_back(friendID);
    }

    for (auto &post: other.posts_) {
        posts_.push_back(new Post(*post));
    }

    for (auto &message: other.receivedMsgs_) {
        receivedMsgs_.push_back(new Message(*message));
    }
}

User& User::operator=(const User& other) {
    if (this != &other) {
        for (auto &post: other.posts_) {
            delete post;
        }

        for (auto &message: other.receivedMsgs_) {
            delete message;
        }

        friends.clear();
        posts_.clear();
        receivedMsgs_.clear();

        us_ = other.us_;
        id_ = other.id_;
        name_ = other.name_;
        for (auto friendID: other.friends) {
            friends.push_back(friendID);
        }

        for (auto &post: other.posts_) {
            posts_.push_back(new Post(*post));
        }

        for (auto &message: other.receivedMsgs_) {
            receivedMsgs_.push_back(new Message(*message));
        }
    }

    return *this;
}


void User::sendMessageInternal(User* user, Message* message, bool checkIsFriend) const {
    // The user we are sending the message to is responsible for freeing the message.
    // In any case that we don't send the message - no one will free it so we must do it    
    if (user == nullptr || message == nullptr || user == this) {
        if (message) {
            delete message;
        }
        throw std::invalid_argument("invalid arguments");
    }

    if (checkIsFriend) {
        bool isFriend = std::find(friends.begin(), friends.end(), user->id_) != friends.end();
        if (!isFriend) {
            delete message;
            throw UserIsNotAFriendException(user->id_);
        }
    }

    user->receiveMessage(message);
}

void User::addFriend(User* user) {
    if (user == nullptr) {
        throw std::invalid_argument("user argument is a nullptr");
    }

    for (auto const& friendID: friends) {
        if (user->id_ == friendID) {
            throw std::invalid_argument("User with the same ID is already a friend");
        }
    }
    friends.push_back(user->id_);
}

void User::removeFriend(User* user) {
    if (user == nullptr) {
        throw std::invalid_argument("user argument is a nullptr");
    }
    // friends is a unique list so we don't need to worry about removing 2 users
    friends.remove(user->id_);
}

void User::post(const string& text) {
    posts_.emplace_back(new Post(text));
}

void User::post(const string& text, Media* media) {
    posts_.emplace_back(new Post(text, media));
}

void User::receiveMessage(Message* message) {
    if (message == nullptr) {
        throw std::invalid_argument("user argument is a nullptr");
    }

    receivedMsgs_.emplace_back(message);
}

void User::sendMessage(User* user, Message* message) const {
    sendMessageInternal(user, message, true);
}

void User::viewReceivedMessages() const {
    std::cout << "viewReceivedMessages: " << id_ << std::endl;
    for (auto &message : receivedMsgs_) {
        std::cout << message->getText() << std::endl;
    }
}

void User::viewFriendsPosts() const {
    std::cout << "viewFriendsPosts: " << id_ << std::endl;
    for (auto friendID: friends) {
        for (auto &post: us_->getUserById(friendID)->posts_) {
            post->printPost();
        }
    }
}

