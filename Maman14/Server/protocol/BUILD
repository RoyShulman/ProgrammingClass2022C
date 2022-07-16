filegroup(
    name = "response_srcs",
    srcs = [
        "common.h",
        "response.cpp",
        "response.h",
    ],
)

filegroup(
    name = "request_srcs",
    srcs = [
        "common.h",
        "request.cpp",
        "request.h",
    ],
)

cc_library(
    name = "libProtocolResponse",
    srcs = [
        ":response_srcs",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
    deps = [
        "//Maman14/Server:libBytearray",
    ],
)

cc_library(
    name = "libProtocolRequest",
    srcs = [
        ":request_srcs",
    ],
    visibility = [
        "//Maman14/Server:__pkg__",
        "//Maman14/Server/tests:__pkg__",
    ],
)

cc_library(
    name = "libProtocol",
    hdrs = [
        "request.h",
        "response.h",
    ],
    visibility = [
        "//Maman14/Server:__pkg__",
    ],
    deps = [
        ":libProtocolRequest",
        ":libProtocolResponse",
    ],
)
