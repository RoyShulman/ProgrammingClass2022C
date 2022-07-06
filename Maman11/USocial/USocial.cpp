#include "USocial.h"
#include <iterator>
#include "USocialExceptions.h"
#include "BusinessUser.h"

USocial::~USocial() {
    clearUsers();
}

USocial::USocial(const USocial& other) {
    copyUsers(other.users_);
}

USocial& USocial::operator=(const USocial& other) {
    if (&other != this) {
        clearUsers();
        copyUsers(other.users_);
    }
    return *this;
}

User* USocial::registerUser(const string& name, bool isBusinessUser) {
    User* newUser;
    if (isBusinessUser) {
        newUser = new BusinessUser;
    } else {
        newUser = new User;
    }
    static unsigned long userID = 1;
    if (userID == 0) {
        throw UserIDOverflowException();
    }

    newUser->us_ = this;
    newUser->id_ = userID++;
    newUser->name_ = name;

    users_.insert({newUser->id_, newUser});

    return newUser;
}

User* USocial::getUserById(unsigned long userID) {
    auto it = users_.find(userID);
    if (it == users_.end()) {
        throw InvalidUserID(userID);    
    }
    return it->second;
}

void USocial::removeUser(User* user) {
    if (user == nullptr) {
        return;
    }
    auto it = users_.erase(user->getId());
    switch (it)
    {
    case 0:
        break;
    case 1:
        delete user;
        break;
    default:
        break;
    }
}

void USocial::clearUsers() {
    for (auto const& it: users_) {
        delete it.second;
    }

    users_.clear();
}

void USocial::copyUsers(const map<unsigned long, User*>& users) {
    for (auto const& it: users) {
        users_.insert({it.first, new User(*it.second)});
    }
}
