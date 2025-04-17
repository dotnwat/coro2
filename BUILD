load("@buildifier_prebuilt//:rules.bzl", "buildifier")
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

refresh_compile_commands(
    name = "cc_gen",
    exclude_external_sources = True,
)

buildifier(
    name = "buildifier",
    exclude_patterns = [
        "./.git/*",
    ],
)

cc_binary(
    name = "concat",
    srcs = [
        "concat.cc",
    ],
    deps = [
        "//runtime:io_uring",
        "//runtime:uv",
    ],
)
