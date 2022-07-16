filegroup(
    name = "srcs",
    srcs = [
        "server.cpp",
    ],
)

cc_library(
    name = "libBytearray",
    srcs = [
        "bytearray.cpp",
    ],
    hdrs = [
        "bytearray.h",
    ],
    visibility = [
        "//Maman14/Server/protocol:__pkg__",
        "//Maman14/Server/tests:__pkg__",
    ],
)

cc_library(
    name = "libUserBackupDirectory",
    srcs = [
        "user_backup_directory.cpp",
    ],
    hdrs = [
        "user_backup_directory.h",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
    deps = [
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "libBackupDirectoryManager",
    srcs = [
        "backup_directory_manager.cpp",
    ],
    hdrs = [
        "backup_directory_manager.h",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
    deps = [
        ":libUserBackupDirectory",
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "libRequestReader",
    srcs = [
        "connection_manager.h",
        "request_reader.cpp",
    ],
    hdrs = [
        "request_reader.h",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
)

cc_library(
    name = "libRequestParser",
    srcs = [
        "request_parser.cpp",
    ],
    hdrs = [
        "request_parser.h",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
    deps = [
        "libRequestReader",
        "//Maman14/Server/protocol:libProtocolRequest",
    ],
)

cc_library(
    name = "libBoostConnectionManager",
    srcs = [
        "boost_connection_manager.cpp",
    ],
    hdrs = [
        "boost_connection_manager.h",
        "connection_manager.h",
    ],
    deps = [
        "@boost//:asio",
    ],
)

cc_library(
    name = "libServer",
    srcs = [
        "server.cpp",
    ],
    hdrs = [
        "server.h",
    ],
    visibility = [
        "//Maman14/Server/tests:__pkg__",
    ],
    deps = [
        ":libBackupDirectoryManager",
        ":libBoostConnectionManager",
        ":libBytearray",
        ":libRequestParser",
        ":libStringUtils",
        "//Maman14/Server/protocol:libProtocol",
        "@boost//:asio",
        "@boost//:log",
        "@boost//:thread",
    ],
)

cc_library(
    name = "libStringUtils",
    srcs = [
        "string_utils.cpp",
    ],
    hdrs = [
        "string_utils.h",
    ],
)

cc_binary(
    name = "server",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":libServer",
    ],
)
