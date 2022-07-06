#pragma once
#include <string>

using std::string;

class Media {
public:
    virtual ~Media() = default;
    virtual void display() const = 0;

    // In order to copy this abstract class we define this function
    virtual Media* clone() const = 0;
protected:
    virtual const string& getDisplayString() const = 0;
};
