load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hunter_lib",
    srcs = [
        "game.cpp",
        "renderer.cpp",
    ],
    hdrs = glob(["*.h"]),
    includes = ["."],
    deps = ["@raylib//:raylib"],
)

cc_binary(
    name = "hunter",
    srcs = ["main.cpp"],
    deps = [":hunter_lib", "//common:launcher"],
    data = glob(["assets/*"]),
)