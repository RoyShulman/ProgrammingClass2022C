load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    sha256 = "51ab2b3d0ef91b2202493a6e259c6151ac175eb6189860c1ba6212ccc57f9d3d",
    strip_prefix = "bazel-compile-commands-extractor-79f8dcae6b451abb97fe76853c867792ac9ac703",

    # Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/79f8dcae6b451abb97fe76853c867792ac9ac703.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

http_archive(
    name = "com_google_googletest",
    sha256 = "ca2d683a5ed38d0c9a5c5ab6a2ab67bc5ce8772444133029b2bfc5c3bf05db2f",
    strip_prefix = "googletest-15460959cbbfa20e66ef0b5ab497367e47fc0a04",
    urls = ["https://github.com/google/googletest/archive/15460959cbbfa20e66ef0b5ab497367e47fc0a04.zip"],
)

_RULES_BOOST_COMMIT = "ea2991b24d68439121130845f7dbef8d1dd383d2"

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "0db14c7eb7acc78cbe601a450d7161e492d7ffc98150775cfef6e2cdf5ec7e6a",
    strip_prefix = "rules_boost-%s" % _RULES_BOOST_COMMIT,
    urls = [
        "https://github.com/nelhage/rules_boost/archive/%s.tar.gz" % _RULES_BOOST_COMMIT,
    ],
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

http_archive(
    name = "cryptopp",
    build_file = "//:cryptopp.BUILD",
    sha256 = "d0d3a28fcb5a1f6ed66b3adf57ecfaed234a7e194e42be465c2ba70c744538dd",
    url = "https://github.com/weidai11/cryptopp/releases/download/CRYPTOPP_8_7_0/cryptopp870.zip",
)
