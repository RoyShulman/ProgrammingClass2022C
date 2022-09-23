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
