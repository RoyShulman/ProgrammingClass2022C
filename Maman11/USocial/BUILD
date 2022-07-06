load("//:variables.bzl", "COPTS")

filegroup(
    name = "media_files",
    srcs = [
        "Media.h",
        "Media.cpp",
        "Photo.h",
        "Photo.cpp",
        "Video.cpp",
        "Video.h",
        "Audio.cpp",
        "Audio.h"
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"]
)

filegroup(
    name = "post_files",
    srcs = [
        "//Maman11/USocial:media_files",
        "Post.cpp",
        "Post.h"
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"]
)

filegroup(
    name = "common_files",
    srcs = [
        "//Maman11/USocial:post_files",
        "User.cpp",
        "User.h",
        "USocial.cpp",
        "USocial.h",
        "Message.cpp",
        "Message.h",
        "USocialExceptions.h",
        "BusinessUser.h",
        "BusinessUser.cpp"
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"]
)

cc_binary(
    name = "USocial",
    srcs = ["main.cpp", "//Maman11/USocial:common_files"],
    copts = COPTS
)
