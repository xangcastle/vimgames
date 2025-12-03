load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "pacman_lib",
    srcs = glob(["*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["*.h"]),
    includes = ["."],
    linkopts = ["-lncurses", "-lpthread"],
    defines = ["MAPS_LOCATION='\"pacman/maps\"'"],
    data = glob(["maps/*.txt"]),
)

cc_binary(
    name = "pacman",
    srcs = ["main.cpp"],
    deps = [":pacman_lib", "//common:launcher"],
    data = glob(["maps/*.txt"]),
)
