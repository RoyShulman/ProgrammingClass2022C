cc_library(
    name = "little_endian_string_stream",
    srcs = ["little_endian_string_stream.cpp"],
    hdrs = ["little_endian_string_stream.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        ":file",
    ],
)

cc_library(
    name = "file",
    srcs = ["file.cpp"],
    hdrs = ["file.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "@boost//:filesystem",
    ],
)
