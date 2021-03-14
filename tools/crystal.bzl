def _crystal_library_impl(ctx):
    src = ctx.file.src
    lib = ctx.outputs.lib
    hpp = ctx.outputs.hpp

    args = ctx.actions.args()
    args.add("lib")
    args.add("-i", src.path)
    args.add("-o", lib.path)

    ctx.actions.run(
        outputs = [lib],
        inputs = [src],
        # progress_message = 'Compiling GLSL "%s"' % vert_in.short_path,
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
        # progress_message = 'Compiling GLSL "%s"' % vert_in.short_path,
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
    },
    outputs = {
        "lib": "%{name}.crystallib",
        "hpp": "%{name}.hpp",
    },
    provides = [DefaultInfo],
)
