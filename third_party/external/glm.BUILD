load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "glm",
    hdrs = glob([
        "glm/**/*.h",
        "glm/**/*.hpp",
        "glm/**/*.inl",
    ]),
    # defines = [
    #     "GLM_FORCE_DEPTH_ZERO_TO_ONE=1",
    # ],
    visibility = [
        "//visibility:public",
    ],
)
