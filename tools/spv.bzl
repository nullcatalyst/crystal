load("@bazel_skylib//lib:paths.bzl", "paths")

_PARTIAL_SPV_DIR = "_spv"
_SPV_FILE_EXT = ".spv"

def _spv_partial(name, stage):
    return "{}/{}".format(_PARTIAL_SPV_DIR, paths.replace_extension(name, "{}.spv".format(stage)))

def _spv_library_impl(ctx):
    name = ctx.attr.name
    srcs = [] + ctx.files.srcs
    tmps = []

    vert_in = ctx.file.vert
    frag_in = ctx.file.frag
    if vert_in not in srcs:
        srcs.append(vert_in)
    if frag_in not in srcs:
        srcs.append(frag_in)

    if vert_in != None:
        vert_out = ctx.actions.declare_file(_spv_partial(name, "vert"))

        args = ctx.actions.args()
        for define in ctx.attr.defines:
            args.add("-D{}".format(define))
        args.add("-Os")  # Optimize for minimum size
        args.add("-V")  # Output spir-v binary
        args.add("-S", "vert")  # Specify the shader stage (vertex)
        args.add("-o", vert_out.path)
        args.add(vert_in.path)

        ctx.actions.run(
            outputs = [vert_out],
            inputs = srcs,
            # progress_message = 'Compiling GLSL "%s"' % vert_in.short_path,
            executable = ctx.executable._compiler,
            arguments = [args],
        )
        tmps.append(vert_out)

    frag_in = ctx.file.frag
    if frag_in != None:
        frag_out = ctx.actions.declare_file(_spv_partial(name, "frag"))

        args = ctx.actions.args()
        for define in ctx.attr.defines:
            args.add("-D{}".format(define))
        args.add("-Os")  # Optimize for minimum size
        args.add("-V")  # Output spir-v binary
        args.add("-S", "frag")  # Specify the shader stage (fragment)
        args.add("-o", frag_out.path)
        args.add(frag_in.path)

        ctx.actions.run(
            outputs = [frag_out],
            inputs = srcs,
            # progress_message = 'Compiling GLSL "%s"' % frag_in.short_path,
            executable = ctx.executable._compiler,
            arguments = [args],
        )
        tmps.append(frag_out)

    out = ctx.actions.declare_file(ctx.attr.out)

    out = ctx.outputs.out
    args = ctx.actions.args()
    args.add("-o", out.path)
    for tmp in tmps:
        args.add(tmp)

    ctx.actions.run(
        outputs = [out],
        inputs = tmps,
        # progress_message = 'Linking SPV "%s"' % out.short_path,
        executable = ctx.executable._linker,
        arguments = [args],
    )

    return [
        DefaultInfo(
            files = depset(direct = [ctx.outputs.out]),
            runfiles = ctx.runfiles(files = [ctx.outputs.out]),
        ),
    ]

spv_library = rule(
    implementation = _spv_library_impl,
    attrs = {
        "out": attr.string(default = "%{name}.spv"),
        "srcs": attr.label_list(allow_files = True),
        "vert": attr.label(allow_single_file = True),
        "frag": attr.label(
            allow_single_file = True,
        ),
        "defines": attr.string_list(),
        "_compiler": attr.label(
            default = Label("@org_khronos_glslang//:glslangValidator"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_linker": attr.label(
            default = Label("@spirv_tools//:spirv-link"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
    },
    outputs = {
        "out": "%{out}",
    },
    provides = [DefaultInfo],
)
