cc_library(
    name = "xml-operations",
    srcs = glob([
        "src/**/*.cc",
    ]),
    hdrs = glob([
        "include/**/*.h",
        "src/**/*.h"
    ]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party:spdlog",
        "@pugixml",
    ],
)
