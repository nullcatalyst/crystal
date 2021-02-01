load("@build_bazel_apple_support//lib:apple_support.bzl", "apple_support")
load("@bazel_skylib//lib:dicts.bzl", "dicts")
load("@bazel_skylib//lib:paths.bzl", "paths")

def _metal_apple_target_triple(ctx):
    platform = ctx.fragments.apple.single_arch_platform
    target_os_version = ctx.attr._xcode_config[apple_common.XcodeVersionConfig].minimum_os_for_platform_type(
        platform.platform_type,
    )

    platform_string = str(platform.platform_type)
    if platform_string == "macos":
        platform_string = "macosx"

    environment = "" if platform.is_device else "-simulator"

    return "air64-apple-{platform}{version}{environment}".format(
        environment = environment,
        platform = platform_string,
        version = target_os_version,
    )

def _metal_library_impl(ctx):
    air_files = []

    # Compile each *.metal file into a single *.air file.
    target = _metal_apple_target_triple(ctx)
    for input_metal in ctx.files.srcs:
        air_file = ctx.actions.declare_file(
            paths.replace_extension(input_metal.basename, ".air"),
        )
        air_files.append(air_file)

        args = [
            "metal",
            "-c",
            "-target",
            target,
            "-ffast-math",
            "-o",
            air_file.path,
            input_metal.path,
        ]

        apple_support.run(
            ctx,
            executable = "/usr/bin/xcrun",
            inputs = [input_metal],
            outputs = [air_file],
            arguments = args,
            mnemonic = "MetalCompile",
        )

    # Compile *.air files into a single *.metallib file, which stores the Metal library.
    output_metallib = ctx.outputs.out

    args = [
        "metallib",
        "-o",
        output_metallib.path,
    ] + [air_file.path for air_file in air_files]

    apple_support.run(
        ctx,
        executable = "/usr/bin/xcrun",
        inputs = air_files,
        outputs = [output_metallib],
        arguments = args,
        mnemonic = "MetallibCompile",
    )

    return [DefaultInfo(files = depset([output_metallib]))]

metal_library = rule(
    attrs = dicts.add(apple_support.action_required_attrs(), {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".metal"],
        ),
        "out": attr.string(
            default = "%{name}.metallib",
        ),
    }),
    implementation = _metal_library_impl,
    fragments = ["apple"],
    outputs = {
        "out": "%{out}",
    },
)
