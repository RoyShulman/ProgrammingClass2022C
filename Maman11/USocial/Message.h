#pragma once
#include <string>

using std::string;

class Message {
public:
    Message(const string& text);
    const string& getText() const { return text_; };
private:
    const string text_;
};
