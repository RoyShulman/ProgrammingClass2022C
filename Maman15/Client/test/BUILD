cc_test(
    name = "little_endian_string_stream_test",
    srcs = ["little_endian_string_stream_test.cc"],
    deps = [
        "//Maman15/Client/util:little_endian_string_stream",
        "@boost//:uuid",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    data = ["resources/file_with_lines.txt"],
    deps = [
        "//Maman15/Client/util:file",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "string_utils_test",
    srcs = ["string_utils_test.cc"],
    deps = [
        "//Maman15/Client/util:string_utils",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "transfer_info_test",
    srcs = ["transfer_info_test.cc"],
    data = [
        "resources/too_short_transfer.info",
        "resources/transfer.info",
    ],
    deps = [
        "//Maman15/Client:transfer_info",
        "//Maman15/Client/util:string_utils",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "private_key_wrapper_test",
    srcs = ["private_key_wrapper_test.cc"],
    deps = [
        "//Maman15/Client/encryption:private_key_wrapper",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "user_info_test",
    srcs = ["user_info_test.cc"],
    data = glob(["resources/*me.info"]),
    deps = [
        "//Maman15/Client:user_info",
        "//Maman15/Client/encryption:private_key_wrapper",
        "@boost//:uuid",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "client_message_test",
    srcs = ["client_message_test.cc"],
    deps = [
        "@//Maman15/Client/protocol:client_message",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "name_string_test",
    srcs = ["name_string_test.cc"],
    deps = [
        "@//Maman15/Client/util:name_string",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "aes_wrapper_test",
    srcs = ["aes_wrapper_test.cc"],
    deps = [
        "@//Maman15/Client/encryption:aes_wrapper",
        "@com_google_googletest//:gtest_main",
    ],
)
