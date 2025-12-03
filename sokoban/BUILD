load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sokoban_lib",
    srcs = glob(["*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["*.h"]),
    includes = ["."],
    linkopts = ["-lncurses", "-lpthread"],
    defines = ["MAPS_LOCATION='\"sokoban/maps\"'"],
    data = glob(["maps/*.txt"]),
)

cc_binary(
    name = "sokoban",
    srcs = ["main.cpp"],
    deps = [":sokoban_lib", "//common:launcher"],
    data = glob(["maps/*.txt"]),
)