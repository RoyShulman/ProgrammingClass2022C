py_library(
    name = "port_info",
    srcs = ["port_info.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "crc32",
    srcs = ["crc32.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "model",
    srcs = ["server_model.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "connection_interface",
    srcs = ["connection_interface.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "encryption_utils",
    srcs = ["encryption_utils.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "file_manager",
    srcs = ["file_manager.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
)

py_library(
    name = "server",
    srcs = ["server.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
    deps = [
        ":connection_interface",
        ":crc32",
        ":encryption_utils",
        ":file_manager",
        ":model",
        "//Maman15/Server/backup_server/protocol:client_message",
        "//Maman15/Server/backup_server/protocol:server_message",
    ],
)

py_library(
    name = "server_manager",
    srcs = ["server_manager.py"],
    visibility = [
        "//Maman15/Server/test:__pkg__",
    ],
    deps = [
        ":server",
        ":port_info"
    ],
)

py_binary(
    name = "main",
    srcs = ["main.py"],
    imports = ["../"],
    deps = [
        ":server_manager",
    ],
)
