filegroup(
    name = "srcs",
    srcs = [
        "credit.cc",
        "credit.h"
    ]
)

cc_binary(
    name = "main",
    srcs = ["//Maman12/Question1:srcs", "main.cc"]
)

cc_library(
    name = "credit",
    srcs = ["//Maman12/Question1:srcs"]
)

cc_test(
    name = "credit-test",
    size = "small",
    srcs = [
        "test.cc"
    ],
    deps = [
        "//Maman12/Question1:credit",
        "@com_google_googletest//:gtest_main",
    ],
)
