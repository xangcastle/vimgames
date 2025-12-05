load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "engine",
    srcs = [
        "core.cpp",
        "graphics.cpp",
        "input.cpp",
    ],
    hdrs = [
        "core.h",
        "engine.h",
        "graphics.h",
        "input.h",
    ],
    includes = ["."],
    linkopts = ["-lncurses"],
)

cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [":engine"],
)
