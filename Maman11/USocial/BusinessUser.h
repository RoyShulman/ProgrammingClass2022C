#pragma once
#include "User.h"

class BusinessUser: public User {
public:
    virtual void sendMessage(User* user, Message* const message) const override;
};
