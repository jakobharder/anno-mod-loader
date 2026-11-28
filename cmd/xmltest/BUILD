package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "xmltest117",
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
        "//third_party:json",
        "//third_party:spdlog",
    ],
)

py_test(
    name = "prepatch-tests",
    srcs = ["tests/prepatch_test.py"],
    main = "tests/prepatch_test.py",
    args = ["$(location :xmltest117)"],
    data = [":xmltest117"],
    python_version = "PY3",
)

py_test(
    name = "defaults-tests",
    srcs = ["tests/defaults_test.py"],
    main = "tests/defaults_test.py",
    args = ["$(location :xmltest117)"],
    data = [":xmltest117"],
    python_version = "PY3",
)
