#include "Audio.h"
#include <iostream>

static const string& AUDIO_DISPLAY_STRING = "audio";

void Audio::display() const {
    Media::display();
}

Audio* Audio::clone() const {
    return new Audio(*this);
};

const string& Audio::getDisplayString() const {
    return AUDIO_DISPLAY_STRING;
}
