cc_library(
    name = "cryptopp",
    srcs = glob(
        ["*.cpp"],
        exclude = ["test.cpp"],
    ),
    hdrs = glob(["*.h"]),
    include_prefix = "cryptopp/",
    textual_hdrs = [
        "ec2n.cpp",
        "algebra.cpp",
        "polynomi.cpp",
        "eprecomp.cpp",
        "strciphr.cpp",
        "eccrypto.cpp",
    ],
    visibility = ["//visibility:public"],
)
