#include "Media.h"
#include <iostream>

void Media::display() const {
    std::cout << getDisplayString() << std::endl;
}
