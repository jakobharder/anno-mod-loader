load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# bazel-skylb 0.8.0 released 2019.03.20 (https://github.com/bazelbuild/bazel-skylib/releases/tag/0.8.0)
skylib_version = "1.3.0"

git_repository(
    name = "io_bazel",
    commit = "15371720ae0c40ffc97b74c871d1b38851ef6410",
    shallow_since = "1638468745 +0100",
    remote = "https://github.com/bazelbuild/bazel.git",
)

http_archive(
    name = "catch2",
    strip_prefix = "Catch2-2.13.10",
    url = "https://github.com/catchorg/Catch2/archive/v2.13.10.tar.gz",
)

http_archive(
    name = "bazel_skylib",
    type = "tar.gz",
    url = "https://github.com/bazelbuild/bazel-skylib/releases/download/{}/bazel-skylib-{}.tar.gz".format(skylib_version, skylib_version),
    sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
)

new_local_repository(
    name = "pugixml",
    build_file = "pugixml.BUILD",
    path = "./third_party/pugixml",
)

load("@io_bazel//src/main/res:winsdk_configure.bzl", "winsdk_configure")
winsdk_configure(name = "local_config_winsdk")
load("@local_config_winsdk//:toolchains.bzl", "register_local_rc_exe_toolchains")
register_local_rc_exe_toolchains()
register_toolchains("@io_bazel//src/main/res:empty_rc_toolchain")
