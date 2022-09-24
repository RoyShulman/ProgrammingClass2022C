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
