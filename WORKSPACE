workspace(name = "crystal")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Skylib
http_archive(
    name = "bazel_skylib",
    sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Abseil
# A custom abseil repo is used that has C++17 set as the default.
http_archive(
    name = "com_google_absl",
    sha256 = "2e66460b6e3dc89d1862fea047b743323c1a69e4e2d6d1dff06bde87fd440f47",
    strip_prefix = "abseil-cpp-35459c0f8c6b599dab00eb9d8d00b7fe32cb1c72",
    urls = [
        "https://github.com/nullcatalyst/abseil-cpp/archive/35459c0f8c6b599dab00eb9d8d00b7fe32cb1c72.tar.gz",
    ],
)
# http_archive(
#     name = "com_google_absl",
#     sha256 = "8100085dada279bf3ee00cd064d43b5f55e5d913be0dfe2906f06f8f28d5b37e",
#     strip_prefix = "abseil-cpp-20190808",
#     urls = [
#         "https://github.com/abseil/abseil-cpp/archive/20190808.tar.gz",
#     ],
# )

# Effcee (required by Abseil)
http_archive(
    name = "com_google_effcee",
    strip_prefix = "effcee-2019.0",
    url = "https://github.com/google/effcee/archive/v2019.0.tar.gz",
)

# Google RE2 (required by Effcee)
http_archive(
    name = "com_googlesource_code_re2",
    sha256 = "b0382aa7369f373a0148218f2df5a6afd6bfa884ce4da2dfb576b979989e615e",
    strip_prefix = "re2-2019-09-01",
    url = "https://github.com/google/re2/archive/2019-09-01.tar.gz",
)

# GoogleTest
http_archive(
    name = "com_google_googletest",
    sha256 = "9bf1fe5182a604b4135edc1a425ae356c9ad15e9b23f9f12a02e80184c3a249c",
    strip_prefix = "googletest-release-1.8.1",
    url = "https://github.com/google/googletest/archive/release-1.8.1.tar.gz",
)

# Google Benchmark
http_archive(
    name = "com_github_google_benchmark",
    sha256 = "3c6a165b6ecc948967a1ead710d4a181d7b0fbcaa183ef7ea84604994966221a",
    strip_prefix = "benchmark-1.5.0",
    url = "https://github.com/google/benchmark/archive/v1.5.0.tar.gz",
)

# GLM
http_archive(
    name = "glm",
    build_file = "//third_party/external:glm.BUILD",
    sha256 = "7d508ab72cb5d43227a3711420f06ff99b0a0cb63ee2f93631b162bfe1fe9592",
    strip_prefix = "glm-0.9.9.8",
    url = "https://github.com/g-truc/glm/archive/0.9.9.8.tar.gz",
)

# SDL2
http_archive(
    name = "sdl2",
    build_file = "//third_party/external:sdl2.BUILD",
    sha256 = "632acd0ddce1639327be9bcfa0e1c5e389c2bdd9df0e4c73349e11a6b7f00570",
    strip_prefix = "SDL-mirror-863c4bd26b51892864c6042ad2db474e11b08fed",
    urls = [
        "https://github.com/spurious/SDL-mirror/archive/863c4bd26b51892864c6042ad2db474e11b08fed.tar.gz",
    ],
)

# Vulkan
load("//tools:vulkan.bzl", "use_local_vulkan")

use_local_vulkan(
    name = "vulkan",
)

# GLSL Validator
http_archive(
    name = "org_khronos_glslang",
    sha256 = "d5cbcd69289da85a775d6a03cedffbe8555e1977610cba04824753db69b0c6e8",
    strip_prefix = "glslang-66daa75f9494fd096856e0989249c6d439c472ad",
    urls = [
        "https://github.com/KhronosGroup/glslang/archive/66daa75f9494fd096856e0989249c6d439c472ad.tar.gz",
    ],
)

# SPIRV Tools
http_archive(
    name = "spirv_tools",
    sha256 = "81c70a92e34b122ec75df1ab7c7d3db891366a86cf8408985d65f3e77396b535",
    strip_prefix = "SPIRV-Tools-4879e3b7851e4c0c165b0364fc748d0be598c0af",
    urls = [
        "https://github.com/KhronosGroup/SPIRV-Tools/archive/4879e3b7851e4c0c165b0364fc748d0be598c0af.tar.gz",
    ],
)

# SPIRV Headers
http_archive(
    name = "spirv_headers",
    sha256 = "df2ad2520be4d95a479fa248921065885bbf435a658349a7fc164ad7b26b68c6",
    strip_prefix = "SPIRV-Headers-1.5.4.raytracing.fixed",
    urls = [
        "https://github.com/KhronosGroup/SPIRV-Headers/archive/1.5.4.raytracing.fixed.tar.gz",
    ],
)
