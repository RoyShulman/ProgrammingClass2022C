py_binary(
    name = "subsection_a",
    srcs = ["pros.py"],
    main = "pros.py",
)

py_binary(
    name = "subsection_b",
    srcs = ["meta.py"],
    main = "meta.py",
)

py_test(
    name = "test",
    srcs = [
        "empty_file.py",
        "test.py",
        "test_file.py",
    ],
    main = "test.py",
    deps = [
        ":subsection_b",
    ],
)
