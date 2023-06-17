package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "annodiff",
    srcs = glob(["src/**/*.cc"]) + glob(["src/**/*.h"]) + glob(["src/**/*.hpp"]),
    defines = ["WIN32_LEAN_AND_MEAN"],
    linkopts = select({
        "@bazel_tools//src/conditions:windows": [],
        "//conditions:default": [
            "-lstdc++fs",
            "-ldl",
        ],
    }),
    deps = [
        "//libs/xml-operations",
        "//third_party:spdlog",
    ],
)
