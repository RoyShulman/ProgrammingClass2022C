load("test_defs.bzl", "py_server_test")

py_server_test(
    name = "test_server_message",
    srcs = [
        "test_server_message.py",
    ],
    deps = ["//Maman15/Server/server/protocol:server_message"],
)

py_server_test(
    name = "test_port_info",
    srcs = [
        "test_port_info.py",
    ],
    deps = ["//Maman15/Server/server:port_info"],
)

py_server_test(
    name = "test_crc32",
    srcs = [
        "test_crc32.py",
    ],
    deps = ["//Maman15/Server/server:crc32"],
)

py_server_test(
    name = "test_server_model",
    srcs = [
        "test_server_model.py",
    ],
    deps = ["//Maman15/Server/server:model"],
)

py_server_test(
    name = "test_client_message",
    srcs = [
        "test_client_message.py",
    ],
    deps = ["//Maman15/Server/server/protocol:client_message",
            "//Maman15/Server/server:connection_interface"],
)
