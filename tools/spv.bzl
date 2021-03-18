def _spv_stage_impl(ctx):
    stage = ctx.attr.stage
    entry = ctx.attr.entry
    defines = ctx.attr.defines
    src = ctx.file.src
    out = ctx.outputs.out

    args = ctx.actions.args()
    for define in defines:
        # define a pre-processor macro
        args.add("-D{}".format(define))

    # optimizes SPIR-V to minimize size
    args.add("-Os")

    # create SPIR-V binary, under Vulkan semantics
    args.add("-V")

    # uses specified stage rather than parsing the file extension choices for <stage> are vert, tesc, tese, geom, frag, or comp
    args.add("-S", stage)

    # specify <name> as the entry-point function name
    args.add("-e", entry)

    # the given shader source function is renamed to be the <name> given in -e
    args.add("--source-entrypoint", "main")

    # save binary to <file>
    args.add("-o", out.path)
    args.add(src.path)

    ctx.actions.run(
        outputs = [out],
        inputs = [src],
        # progress_message = 'Compiling GLSL "%s"' % vert_in.short_path,
        executable = ctx.executable._compiler,
        arguments = [args],
    )

    return [
        DefaultInfo(
            files = depset(direct = [out]),
            runfiles = ctx.runfiles(files = [out]),
        ),
    ]

spv_stage = rule(
    implementation = _spv_stage_impl,
    attrs = {
        "out": attr.string(mandatory = True),
        "src": attr.label(allow_single_file = True),
        "stage": attr.string(mandatory = True),
        "entry": attr.string(default = "main"),
        "defines": attr.string_list(),
        "_compiler": attr.label(
            default = Label("@org_khronos_glslang//:glslangValidator_opt"),
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

def spv_vertex(**kwargs):
    return spv_stage(
        out = "{}.spv".format(kwargs["name"]),
        stage = "vert",
        **kwargs
    )

def spv_fragment(**kwargs):
    return spv_stage(
        out = "{}.spv".format(kwargs["name"]),
        stage = "frag",
        **kwargs
    )

def _spv_library_impl(ctx):
    srcs = ctx.files.srcs
    out = ctx.outputs.out

    args = ctx.actions.args()
    args.add("-o", out.path)
    for src in srcs:
        args.add(src.path)

    ctx.actions.run(
        outputs = [out],
        inputs = srcs,
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
