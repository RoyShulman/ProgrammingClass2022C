load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
        # "@//... - //toolchain/...": "--config=local", This didn't work :(
        "//Maman15/...": "--config=local",
    },
)
