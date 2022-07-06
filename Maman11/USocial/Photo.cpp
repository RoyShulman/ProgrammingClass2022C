#include "Photo.h"
#include <iostream>

static const string& PHOTO_DISPLAY_STRING = "image";

void Photo::display() const {
    Media::display();
}

Photo* Photo::clone() const {
    return new Photo(*this);
};

const string& Photo::getDisplayString() const {
    return PHOTO_DISPLAY_STRING;
}
