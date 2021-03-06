load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")
load("@rules_pkg//:pkg.bzl", "pkg_deb", "pkg_tar")

config_setting(
    name = "macos_x86_64",
    values = {
        "apple_platform_type": "macos",
        "cpu": "darwin",
    },
    visibility = ["//visibility:private"],
)

config_setting(
    name = "macos_arm64",
    values = {
        "apple_platform_type": "macos",
        "cpu": "darwin_arm64",
    },
    visibility = ["//visibility:private"],
)

selects.config_setting_group(
    name = "macos",
    match_any = [
        ":macos_x86_64",
        ":macos_arm64",
    ],
)

config_setting(
    name = "opt",
    values = {"compilation_mode": "opt"},
    visibility = ["//visibility:private"],
)

COPTS = [
    "-std=c++17",
    "-Werror",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wvla",
] + select({
    "//:opt": [
        "-Ofast",
        "-flto",
    ],
    "//conditions:default": [],
})

LINKOPTS = select({
    "//:opt": [
        "-O3",
        "-flto",
        "-s",
    ],
    "//conditions:default": [],
})

genrule(
    name = "buildinfo",
    srcs = [
        "CMakeLists.txt",
    ] + glob([
        "cmake/**/*",
        "**/*.in",
    ]),
    outs = ["include/buildinfo.h"],
    cmd = "cmake -S $$(dirname $(location CMakeLists.txt)) -B $(RULEDIR) -DBUILDINFO_ONLY=ON",
)

filegroup(
    name = "included_headers",
    srcs = ["include/buildinfo.h"] + glob(["include/**/*.h"]),
)

configure_make(
    name = "xmlrpc",
    configure_env_vars = select({
        "//:macos": {
            "AR": "",
        },
        "//conditions:default": {},
    }),
    configure_in_place = True,
    configure_options = [
        "--disable-wininet-client",
        "--disable-curl-client",
        "--disable-libwww-client",
        "--disable-abyss-server",
        "--disable-cgi-server",
        "--disable-cplusplus",
    ],
    lib_source = "@xmlrpc//:all",
    make_commands = select({
        "//:macos": [
            "CFLAGS='-D_DARWIN_C_SOURCE' make",
            "make install",
        ],
        "//conditions:default": [
            "make",
            "make install",
        ],
    }),
    static_libraries = [
        "libxmlrpc_server.a",
        "libxmlrpc.a",
        "libxmlrpc_xmlparse.a",
        "libxmlrpc_xmltok.a",
        "libxmlrpc_util.a",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "rtorrent_common",
    srcs = glob(
        ["src/**/*.cc"],
        exclude = ["src/main.cc"],
    ) + ["//:included_headers"],
    copts = COPTS,
    includes = ["include"],
    linkopts = LINKOPTS + select({
        "//:macos": [
            "-lxml2",
            "-liconv",
            "-lncurses",
        ],
        "//conditions:default": [
            "-lpthread",
            "-lstdc++",
        ],
    }),
    deps = [
        "@curl",
        "xmlrpc",
        "@libtorrent//:torrent",
    ] + select({
        "//:macos": [],
        "//conditions:default": [
            "@ncurses//:ncursesw",
        ],
    }),
)

cc_binary(
    name = "rtorrent",
    srcs = [
        "src/main.cc",
        "//:included_headers",
    ],
    copts = COPTS,
    includes = ["include"],
    linkopts = LINKOPTS,
    linkstatic = True,
    deps = [
        "//:rtorrent_common",
    ],
)

cc_test(
    name = "rtorrent_test",
    srcs = glob([
        "test/**/*.cc",
    ]) + ["//:included_headers"],
    copts = COPTS,
    includes = ["include"],
    linkopts = LINKOPTS + select({
        "//:macos": [],
        "//conditions:default": ["-lcppunit"],
    }),
    deps = [
        "//:rtorrent_common",
    ] + select({
        "//:macos": [
            "@cppunit",
        ],
        "//conditions:default": [],
    }),
)

pkg_tar(
    name = "rtorrent-bin",
    srcs = ["//:rtorrent"],
    mode = "0755",
    package_dir = "/usr/bin",
    strip_prefix = "/",
)

pkg_tar(
    name = "rtorrent-cfg",
    srcs = ["doc/rtorrent.rc"],
    mode = "0644",
    package_dir = "/etc/rtorrent",
    strip_prefix = "/doc",
)

pkg_tar(
    name = "rtorrent-service",
    srcs = ["doc/rtorrent@.service"],
    mode = "0644",
    package_dir = "/etc/systemd/system",
    strip_prefix = "/doc",
)

pkg_tar(
    name = "rtorrent-deb-data",
    extension = "tar.gz",
    deps = [
        ":rtorrent-bin",
        ":rtorrent-cfg",
        ":rtorrent-service",
    ],
)

pkg_deb(
    name = "rtorrent-deb",
    architecture = "all",
    conffiles = [
        "/etc/rtorrent/rtorrent.rc",
        "/etc/systemd/system/rtorrent@.service",
    ],
    data = ":rtorrent-deb-data",
    description = "a stable and high-performance BitTorrent client",
    homepage = "https://github.com/jesec/rtorrent",
    maintainer = "Jesse Chan <jc@linux.com>",
    package = "rtorrent",
    version = "0.9.8-jc+master",
)
