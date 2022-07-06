#pragma once
#include <exception>
#include <stdexcept>

class UserIDOverflowException : public std::exception {
public:
    virtual const char* what() const noexcept { return "User ID overflowed!"; };
};

// We inherit from runtime_error because we want to a formatted string when throwing the exception
class InvalidUserID : public std::runtime_error {
public:
    InvalidUserID(unsigned long userID) : 
        std::runtime_error("ID: " + std::to_string(userID) + " is invalid!") {};
};

class UserIsNotAFriendException : public std::runtime_error {
public:
    UserIsNotAFriendException(unsigned long userID) : 
        std::runtime_error("User with ID: " + std::to_string(userID) + " is not a friend!") {};
};
