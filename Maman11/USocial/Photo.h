#pragma once
#include <string>
#include "Media.h"

using std::string;

class Photo : public Media {
public:
    virtual ~Photo() = default;

    virtual void display() const override;
    virtual Photo* clone() const override;
protected:
    virtual const string& getDisplayString() const override;
};
