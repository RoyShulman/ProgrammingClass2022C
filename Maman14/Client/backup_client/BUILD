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
    name = "client_srcs",
    srcs = [
        "client.py",
        "server_info.py",
    ],
)

py_library(
    name = "libclient",
    srcs = [":client_srcs"],
    visibility = ["//Maman14/Client/tests:__pkg__"],
)

py_binary(
    name = "client",
    srcs = [
        "main.py",
    ],
    deps = [
        ":libclient",
    ],
)
