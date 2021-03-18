def _crystal_library_impl(ctx):
    src = ctx.file.src
    lib = ctx.outputs.lib
    hpp = ctx.outputs.hpp

    args = ctx.actions.args()
    args.add("lib")
    args.add("-i", src.path)
    args.add("-o", lib.path)
    args.add("--glslang_validator", ctx.executable._glslangValidator.path)
    args.add("--spirv_link", ctx.executable._spirvLink.path)

    ctx.actions.run(
        outputs = [lib],
        inputs = [
            src,
            ctx.executable._glslangValidator,
            ctx.executable._spirvLink,
        ],
        executable = ctx.executable._compiler,
        arguments = [args],
    )

    args = ctx.actions.args()
    args.add("cpp")
    args.add("-i", src.path)
    args.add("-o", hpp.path)

    ctx.actions.run(
        outputs = [hpp],
        inputs = [src],
        executable = ctx.executable._compiler,
        arguments = [args],
    )

    return [
        DefaultInfo(
            files = depset(direct = [
                lib,
                hpp,
            ]),
            runfiles = ctx.runfiles(files = [lib]),
        ),
    ]

crystal_library = rule(
    implementation = _crystal_library_impl,
    attrs = {
        "src": attr.label(allow_single_file = True),
        "_compiler": attr.label(
            default = Label("//crystal/compiler/cli"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_glslangValidator": attr.label(
            default = Label("@org_khronos_glslang//:glslangValidator_opt"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_spirvLink": attr.label(
            default = Label("@spirv_tools//:spirv-link"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
    },
    outputs = {
        "lib": "%{name}.crystallib",
        "hpp": "%{name}.hpp",
    },
    provides = [DefaultInfo],
)
