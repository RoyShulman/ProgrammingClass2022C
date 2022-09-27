cc_library(
    name = "client_message",
    srcs = ["client_message.cpp"],
    hdrs = ["client_message.h"],
    visibility = [
        "//Maman15/Client:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "//Maman15/Client/util:little_endian_string_stream",
        "//Maman15/Client/util:name_string",
        "@boost//:uuid",
    ],
)

cc_library(
    name = "server_message",
    srcs = ["server_message.cpp"],
    hdrs = ["server_message.h"],
    visibility = [
        "//Maman15/Client:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "//Maman15/Client/util:little_endian_string_stream",
        "//Maman15/Client/util:name_string",
        "@boost//:uuid",
    ],
)

cc_library(
    name = "protocol",
    visibility = [
        "//Maman15/Client:__pkg__",
    ],
    deps = [
        ":client_message",
        ":server_message",
    ],
)
