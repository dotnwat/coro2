cc_library(
    name = "runtime",
    srcs = [
        "backend.cc",
        "runtime.cc",
    ],
    hdrs = [
        "backend.h",
        "runtime.h",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "io_uring",
    srcs = [
        "io_uring.cc",
    ],
    hdrs = [
        "io_uring.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":runtime",
        "@liburing",
    ],
)

cc_library(
    name = "uv",
    srcs = [
        "uv.cc",
    ],
    hdrs = [
        "uv.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":runtime",
        "@libuv",
    ],
)
