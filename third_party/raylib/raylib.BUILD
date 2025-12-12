load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "raylib",
    srcs = ["lib/libraylib.a"],
    hdrs = ["include/raylib.h"],
    includes = ["include"],
    linkopts = [
        "-framework CoreVideo",
        "-framework IOKit",
        "-framework Cocoa",
        "-framework GLUT",
        "-framework OpenGL",
    ],
)
