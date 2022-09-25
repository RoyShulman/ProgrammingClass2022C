cc_library(
    name = "little_endian_string_stream",
    srcs = ["little_endian_string_stream.cpp"],
    hdrs = ["little_endian_string_stream.h"],
    visibility = [
        "//Maman15/Client/protocol:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        ":file",
        ":name_string",
        "@boost//:uuid",
    ],
)

cc_library(
    name = "file",
    srcs = ["file.cpp"],
    hdrs = ["file.h"],
    visibility = [
        "//Maman15/Client:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "string_utils",
    srcs = ["string_utils.cpp"],
    hdrs = ["string_utils.h"],
    visibility = [
        "//Maman15/Client:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "@boost//:asio",
    ],
)

cc_library(
    name = "name_string",
    srcs = ["name_string.cpp"],
    hdrs = ["name_string.h"],
    visibility = [
        "//Maman15/Client:__pkg__",
        "//Maman15/Client/protocol:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
)
