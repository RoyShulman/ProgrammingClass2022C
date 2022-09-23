cc_test(
    name = "little_endian_string_stream_test",
    srcs = ["little_endian_string_stream_test.cc"],
    deps = [
        "//Maman15/Client/util:little_endian_string_stream",
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
