_VK_SDK_PATH = "VK_SDK_PATH"
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
    ctx.execute(["cmd.exe", "/C", bat], timeout=10)

# def _copy_bash(ctx, src, dst):
#     ctx.actions.run_shell(
#         tools = [src],
#         outputs = [dst],
#         command = "cp -f \"$1\" \"$2\"",
#         arguments = [src, dst],
#         mnemonic = "CopyFile",
#         progress_message = "Copying files",
#         use_default_shell_env = True,
#     )

def _use_local_vulkan(ctx):
    env = ctx.os.environ
    if _VK_SDK_PATH not in env:
        fail('Environment variable "{}" not set'.format(_VK_SDK_PATH))

    name = ctx.name or _VK_DEFAULT_WORKSPACE_NAME
    ctx.file("WORKSPACE", 'workspace(name = "{}")\n'.format(name))

    sdk_path = env[_VK_SDK_PATH]
    include_path = "include"
    lib_path = "lib"

    if "windows" in ctx.os.name:
        # Replace all backslashes in the path, try to prevent accidental escaping.
        sdk_path = sdk_path.replace("\\", "/")
        if sdk_path.endswith("/"):
            sdk_path = sdk_path[:-1]

        include_path = "include"
        lib_path = "lib/vulkan-1.lib"

        _copy_cmd(ctx, "lib", sdk_path + "/Lib/vulkan-1.lib", "lib/")
        _copy_cmd(ctx, "include", sdk_path + "/Include/vulkan", "include/vulkan/")

    ctx.file("BUILD.bazel", _VK_BUILD_FILE_TMPL.format(
        include = include_path,
        lib = lib_path,
    ))

use_local_vulkan = repository_rule(
    implementation = _use_local_vulkan,
    environ = [_VK_SDK_PATH],
)
