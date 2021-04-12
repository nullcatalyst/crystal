load("@bazel_skylib//lib:paths.bzl", "paths")

_VK_SDK_PATH = "VK_SDK_PATH"
_GGP_SDK_PATH = "GGP_SDK_PATH"
_VK_DEFAULT_WORKSPACE_NAME = "vulkan"
_VK_BUILD_FILE_TMPL = """
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "vulkan",
    srcs = [
        "{lib}",
    ],
    hdrs = glob([
        "{include}/vulkan/**/*.h",
        "{include}/vulkan/**/*.hpp",
    ]),
    strip_include_prefix = "{include}",
    visibility = [
        "//visibility:public",
    ],
)
"""

def _string_path_basename(s):
    SEP = "/"
    index = s.rfind(SEP)
    if index:
        s = s[index + len(SEP):]
    index = s.rfind(".")
    if index:
        s = s[:index]
    return s

def _copy_cmd(ctx, name, src, dst):
    # Most Windows binaries built with MSVC use a certain argument quoting
    # scheme. Bazel uses that scheme too to quote arguments. However,
    # cmd.exe uses different semantics, so Bazel's quoting is wrong here.
    # To fix that we write the command to a .bat file so no command line
    # quoting or escaping is required.
    bat = "copy-%s-cmd.bat" % (name)
    ctx.file(
        bat,
        # Do not use lib/shell.bzl's shell.quote() method, because that uses
        # Bash quoting syntax, which is different from cmd.exe's syntax.
        content = "xcopy /s \"%s\" \"%s\" >NUL" % (
            src.replace("/", "\\"),
            dst.replace("/", "\\"),
        ),
    )
    ctx.execute(["cmd.exe", "/C", bat], timeout = 10)

def _copy_bash(ctx, src, dst):
    ctx.execute(["mkdir", "-p", paths.dirname(dst)], timeout = 10)
    ctx.execute([
        "cp",
        "-f",  # Force (replace existing files)
        "-r",  # Recursive
        "-L",  # Follow symlinks
        src,
        dst,
    ], timeout = 10)

def _use_local_vulkan(ctx):
    env = ctx.os.environ
    if (_VK_SDK_PATH not in env) and (_GGP_SDK_PATH not in env):
        fail('Environment variable "{}" not set'.format(_VK_SDK_PATH))

    name = ctx.name or _VK_DEFAULT_WORKSPACE_NAME
    ctx.file("WORKSPACE", """
workspace(name = "{}")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
    ],
)
""".format(name))

    sdk_path = env[_VK_SDK_PATH] if _VK_SDK_PATH in env else paths.join(env[_GGP_SDK_PATH], "sysroot")
    include_path = "include"
    lib_path = "lib/libvulkan.so"

    if "windows" in ctx.os.name:
        # Replace all backslashes in the path, try to prevent accidental escaping.
        sdk_path = sdk_path.replace("\\", "/")
        if sdk_path.endswith("/"):
            sdk_path = sdk_path[:-1]

        include_path = "include"
        lib_path = "lib/vulkan-1.lib"

        _copy_cmd(ctx, "lib", sdk_path + "/Lib/vulkan-1.lib", "lib/")
        _copy_cmd(ctx, "include", sdk_path + "/Include/vulkan", "include/vulkan/")
    else:
        _copy_bash(ctx, sdk_path + "/lib/libvulkan.so", lib_path)
        _copy_bash(ctx, sdk_path + "/usr/include/vulkan", "include/vulkan")

    ctx.file("BUILD.bazel", _VK_BUILD_FILE_TMPL.format(
        include = include_path,
        lib = lib_path,
    ))

use_local_vulkan = repository_rule(
    implementation = _use_local_vulkan,
    environ = [_VK_SDK_PATH],
)
