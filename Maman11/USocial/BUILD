filegroup(
    name = "media_files",
    srcs = [
        "Audio.cpp",
        "Audio.h",
        "Media.cpp",
        "Media.h",
        "Photo.cpp",
        "Photo.h",
        "Video.cpp",
        "Video.h",
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"],
)

filegroup(
    name = "post_files",
    srcs = [
        "Post.cpp",
        "Post.h",
        "//Maman11/USocial:media_files",
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"],
)

filegroup(
    name = "common_files",
    srcs = [
        "BusinessUser.cpp",
        "BusinessUser.h",
        "Message.cpp",
        "Message.h",
        "USocial.cpp",
        "USocial.h",
        "USocialExceptions.h",
        "User.cpp",
        "User.h",
        "//Maman11/USocial:post_files",
    ],
    visibility = ["//Maman11/USocial/test:__pkg__"],
)

cc_binary(
    name = "USocial",
    srcs = [
        "main.cpp",
        "//Maman11/USocial:common_files",
    ],
)
