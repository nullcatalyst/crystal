load("@bazel_skylib//lib:paths.bzl", "paths")

def _lemon_c_library_impl(ctx):
    name = ctx.attr.name
    outs = [
        ctx.outputs.c,
        ctx.outputs.h,
        ctx.outputs.out,
    ]
    args = ctx.actions.args()

    # args.add("-q")
    args.add("-T{}".format(ctx.file._template.path))
    args.add("-d{}".format(paths.dirname(ctx.outputs.c.path)))
    args.add(ctx.file.src.path)

    ctx.actions.run(
        outputs = outs,
        inputs = [ctx.file.src],
        executable = ctx.executable._compiler,
        arguments = [args],
    )

    return [
        DefaultInfo(
            files = depset(direct = outs),
        ),
    ]

lemon_c_library = rule(
    implementation = _lemon_c_library_impl,
    attrs = {
        "src": attr.label(allow_single_file = [".lemon"]),
        "_compiler": attr.label(
            default = Label("//third_party/lemon"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_template": attr.label(
            default = "//third_party/lemon:lempar.c",
            allow_single_file = True,
        ),
    },
    outputs = {
        "cpp": "%{name}.cpp",
        "hpp": "%{name}.hpp",
        "out": "%{name}.out",
    },
    provides = [DefaultInfo],
)

def _lemon_cpp_library_impl(ctx):
    name = ctx.attr.name
    outs = [
        ctx.outputs.cpp,
        ctx.outputs.hpp,
        ctx.outputs.out,
    ]
    args = ctx.actions.args()

    # args.add("-q")
    args.add("-X")
    args.add("-T{}".format(ctx.file._template.path))
    args.add("-d{}".format(paths.dirname(ctx.outputs.cpp.path)))
    args.add(ctx.file.src.path)

    ctx.actions.run(
        outputs = outs,
        inputs = [
            ctx.file.src,
            ctx.file._template,
        ],
        executable = ctx.executable._compiler,
        arguments = [args],
    )

    return [
        DefaultInfo(
            files = depset(direct = outs),
        ),
    ]

lemon_cpp_library = rule(
    implementation = _lemon_cpp_library_impl,
    attrs = {
        "src": attr.label(allow_single_file = [".lemon"]),
        "_compiler": attr.label(
            default = Label("//third_party/lemon"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_template": attr.label(
            default = "//third_party/lemon:lempar.c",
            allow_single_file = True,
        ),
    },
    outputs = {
        "cpp": "%{name}.cpp",
        "hpp": "%{name}.hpp",
        "out": "%{name}.out",
    },
    provides = [DefaultInfo],
)
