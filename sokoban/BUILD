load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sokoban_lib",
    srcs = glob(["*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["*.h"]),
    includes = ["."],
    linkopts = [],
    defines = ["MAPS_LOCATION='\"sokoban/maps\"'"],
    deps = [
        "@raylib//:raylib",
    ],
)

cc_binary(
    name = "sokoban",
    srcs = ["main.cpp"],
    deps = [":sokoban_lib", "//common:launcher", "@raylib//:raylib"],
    data = glob(["maps/*.txt", "assets/*.png"]),
)