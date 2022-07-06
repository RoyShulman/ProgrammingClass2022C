#pragma once
#include <string>
#include "Media.h"

using std::string;

class Audio : public Media {
public:
    virtual ~Audio() = default;
    virtual void display() const override;
    virtual Audio* clone() const override;
protected:
    virtual const string& getDisplayString() const override;
};

