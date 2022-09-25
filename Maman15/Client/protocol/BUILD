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
        "@boost//:uuid",
    ],
)
