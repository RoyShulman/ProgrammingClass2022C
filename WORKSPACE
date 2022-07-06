load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",

    # Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/79f8dcae6b451abb97fe76853c867792ac9ac703.tar.gz",
    strip_prefix = "bazel-compile-commands-extractor-79f8dcae6b451abb97fe76853c867792ac9ac703",
    sha256 = "51ab2b3d0ef91b2202493a6e259c6151ac175eb6189860c1ba6212ccc57f9d3d"

)
load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")
hedron_compile_commands_setup()


http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/15460959cbbfa20e66ef0b5ab497367e47fc0a04.zip"],
  sha256 = "ca2d683a5ed38d0c9a5c5ab6a2ab67bc5ce8772444133029b2bfc5c3bf05db2f",
  strip_prefix = "googletest-15460959cbbfa20e66ef0b5ab497367e47fc0a04",
)
