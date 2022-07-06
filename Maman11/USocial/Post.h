#pragma once
#include <string>
#include "Media.h"

using std::string;

class Post {
public:
    Post(const string& text);
    Post(const string& text, Media* media);

    ~Post();

    Post(const Post& other);

    Post& operator=(const Post& other);

    const string& getText() const { return text_; };
    const Media* getMedia() const { return media_; };

    void printPost() const;

private:
    string text_;
    Media* media_;
};
