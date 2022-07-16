py_binary(
    name = "subsection_a",
    srcs = ["fruit.py"],
    main = "fruit.py"
)

py_binary(
    name = "subsection_b",
    srcs = ["bank.py"],
    main = "bank.py"
)

py_test(
    name = "test",
    srcs = ["test.py"],
    main = "test.py",
    deps = [
        "//Maman13/Question2:subsection_a"
    ]
)
