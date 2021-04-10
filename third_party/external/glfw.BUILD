load("@rules_cc//cc:defs.bzl", "cc_library")

# mkdir build
# cd build
# cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DSDL_TEST=OFF -DSDL_SHARED=OFF -DSDL_STATIC=ON ..
# cmake --build .
# open compile_commands.json

_GLFW_HDRS = glob([
    "include/**/*.h",
])

_GLFW_SRC_HDRS = glob([
    "src/**/*.h",
])

_GLFW_SRCS = select({
    "@//:windows": [
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c",
        "src/win32_init.c",
        "src/win32_joystick.c",
        "src/win32_monitor.c",
        "src/win32_time.c",
        "src/win32_thread.c",
        "src/win32_window.c",
        "src/wgl_context.c",
        "src/egl_context.c",
        "src/osmesa_context.c",
    ],
    "@//:macos": [
        # All macos srcs are defined in the :sdl2_macos target.
    ],
})

_GLFW_COPTS = select({
    "@//:windows": [
        "/W3",
        # "/MD",
        # "/O2",
        "/Ob2",
        "/GS-",
        # "/arch:SSE",
    ],
    "@//:macos": [
    ],
})

_GLFW_DEFINES = select({
    "@//:windows": [
        "_CRT_SECURE_NO_WARNINGS",
        "_GLFW_WIN32",
        "_GLFW_VULKAN_STATIC",
        "_UNICODE",
    ],
    "@//:macos": [
    ],
})

_GLFW_INCLUDES = select({
    "@//:windows": [
        "include",
    ],
    "@//:macos": [
        "include",
    ],
})

_GLFW_LINKOPTS = select({
    "@//:windows": [
    ],
    "@//:macos": [
    ],
})

cc_library(
    name = "hdrs",
    hdrs = _GLFW_HDRS,
    defines = _GLFW_DEFINES,
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "glfw",
    srcs = _GLFW_SRC_HDRS + _GLFW_SRCS,
    hdrs = _GLFW_HDRS,
    copts = _GLFW_COPTS,
    defines = _GLFW_DEFINES,
    includes = _GLFW_INCLUDES,
    linkopts = _GLFW_LINKOPTS,
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@vulkan",
    ],
)
