load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sokoban_lib",
    srcs = glob(["*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["*.h"]),
    includes = ["."],
    linkopts = ["-lncurses", "-lpthread"],
)

cc_binary(
    name = "sokoban",
    srcs = ["main.cpp"],
    deps = [":sokoban_lib"],
)