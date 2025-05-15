package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "xmltest2",
    srcs = glob(["src/**/*.cc"]) + glob(["src/**/*.h"]),
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
