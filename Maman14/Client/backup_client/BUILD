load("backup_client_defs.bzl", "backup_client_binary")

filegroup(
    name = "server_info_srcs",
    srcs = [
        "server_info.py",
    ],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

filegroup(
    name = "backup_info_srcs",
    srcs = [
        "backup_info.py",
    ],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

filegroup(
    name = "protocol_srcs",
    srcs = [
        "connection_manager.py",
        "protocol.py",
        "response_reader.py",
    ],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

filegroup(
    name = "client_srcs",
    srcs = [
        "client.py",
        "connection_manager.py",
        ":backup_info_srcs",
        ":protocol_srcs",
        ":server_info_srcs",
    ],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

py_library(
    name = "libclient",
    srcs = [":client_srcs"],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

backup_client_binary(
    name = "client",
    srcs = [
        "main.py",
    ],
    data = [
        ":backup.info",
        ":server.info",
    ],
    main = "main.py",
    deps = [
        ":libclient",
    ],
)
