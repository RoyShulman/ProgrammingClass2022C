py_binary(
    name = "subsection_a",
    srcs = ["past.py"],
    main = "past.py"
)

py_binary(
    name = "subsection_b",
    srcs = ["rain.py"],
    main = "rain.py"
)

py_test(
    name = "test",
    srcs = ["test.py"],
    main = "test.py",
    deps = [
        "//Maman13/Question1:subsection_a",
        "//Maman13/Question1:subsection_b",
    ],
)
