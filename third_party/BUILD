cc_library(
    name = "json",
    hdrs = glob(["json/single_include/json.hpp"]),
    includes = [
        "json/single_include",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "spdlog",
    defines = ["SPDLOG_WCHAR_FILENAMES"] + select({
        "@bazel_tools//src/conditions:windows": ["SPDLOG_WCHAR_TO_UTF8_SUPPORT"],
        "//conditions:default": [],
    }),
    hdrs = glob(["spdlog/include/**/*.h"]),
    includes = [
        "spdlog/include",
    ],
    visibility = ["//visibility:public"],
)
