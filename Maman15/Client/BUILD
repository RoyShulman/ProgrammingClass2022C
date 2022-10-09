cc_library(
    name = "transfer_info",
    srcs = ["transfer_info.cpp"],
    hdrs = ["transfer_info.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "//Maman15/Client/util:file",
        "//Maman15/Client/util:string_utils",
        "@boost//:asio",
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "user_info",
    srcs = ["user_info.cpp"],
    hdrs = ["user_info.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "//Maman15/Client/encryption:private_key_wrapper",
        "//Maman15/Client/util:file",
        "@boost//:filesystem",
        "@boost//:uuid",
    ],
)

cc_library(
    name = "incoming_message_reader",
    srcs = ["incoming_message_reader.cpp"],
    hdrs = ["incoming_message_reader.h"],
    visibility = [
        "//Maman15/Client/protocol:__pkg__",
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        ":connection_manager",
        "@boost//:asio",
    ],
)

cc_library(
    name = "connection_manager",
    srcs = ["connection_manager.cpp"],
    hdrs = ["connection_manager.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        "@boost//:asio",
    ],
)

cc_library(
    name = "client",
    srcs = ["client.cpp"],
    hdrs = ["client.h"],
    visibility = [
        "//Maman15/Client/test:__pkg__",
    ],
    deps = [
        ":connection_manager",
        ":incoming_message_reader",
        ":transfer_info",
        ":user_info",
        "//Maman15/Client/encryption:aes_wrapper",
        "//Maman15/Client/protocol",
        "//Maman15/Client/util:crc32",
        "@boost//:log",
    ],
)

cc_binary(
    name = "client_main",
    srcs = ["main.cpp"],
    deps = [
        ":client",
        "@boost//:log",
    ],
)
