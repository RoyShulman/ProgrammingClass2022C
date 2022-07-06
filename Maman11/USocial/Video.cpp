#include "Video.h"
#include <iostream>

static const string& VIDEO_DISPLAY_STRING = "video";

void Video::display() const {
    Media::display();
}

Video* Video::clone() const {
    return new Video(*this);
}

const string& Video::getDisplayString() const {
    return VIDEO_DISPLAY_STRING;
}
