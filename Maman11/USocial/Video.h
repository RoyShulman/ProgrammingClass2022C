#pragma once
#include <string>
#include "Media.h"

using std::string;

class Video : public Media {
public:
    virtual ~Video() = default;

    virtual void display() const override;
    virtual Video* clone() const override;
protected:
    virtual const string& getDisplayString() const override;
};
