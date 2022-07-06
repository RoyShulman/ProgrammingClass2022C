#include "Post.h"

#include <iostream>
#include <string>

using std::string;

Post::Post(const string& text) : text_(text), media_(nullptr) {}

Post::Post(const string& text, Media* const media) : text_(text), media_(media) {}

Post::~Post() {
    if (media_ != nullptr) {
        delete media_;
    }
}

Post::Post(const Post& other) : text_(other.text_) {
    if (other.media_ == nullptr) {
        media_ = nullptr;
    } else {
        media_ = other.media_->clone();
    }   
}

Post& Post::operator=(const Post& other) {
    if (this != &other) {
        text_ = other.text_;
        
        delete media_;  // deleting a nullptr is ok and does nothing
        media_ = other.media_->clone();
    }
    return *this;
}

void Post::printPost() const {
    std::cout << "============" << std::endl;
    if (media_ == nullptr) {
        std::cout << getText() << std::endl;
    } else {
        std::cout << getText() << std::endl;
        getMedia()->display();
    }
    std::cout << "============" << std::endl;
}
