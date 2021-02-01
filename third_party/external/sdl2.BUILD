load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@bazel_skylib//rules:copy_file.bzl", "copy_file")

# mkdir build
# cd build
# cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DSDL_TEST=OFF -DSDL_SHARED=OFF -DSDL_STATIC=ON ..
# cmake --build .
# open compile_commands.json

_SDL_HDRS = [
    ":sdl2_config",
] + glob(
    [
        "include/**/*.h",
    ],
    exclude = [
        "include/SDL_config.h",
    ],
)

_SDL_SRC_HDRS = glob([
    "src/**/*.h",
])

_SDL_SRCS = select({
    "@//:windows": [
        "src/SDL.c",
        "src/SDL_assert.c",
        "src/SDL_dataqueue.c",
        "src/SDL_error.c",
        "src/SDL_hints.c",
        "src/SDL_log.c",
        "src/atomic/SDL_atomic.c",
        "src/atomic/SDL_spinlock.c",
        "src/audio/SDL_audio.c",
        "src/audio/SDL_audiocvt.c",
        "src/audio/SDL_audiodev.c",
        "src/audio/SDL_audiotypecvt.c",
        "src/audio/SDL_mixer.c",
        "src/audio/SDL_wave.c",
        "src/audio/directsound/SDL_directsound.c",
        "src/audio/disk/SDL_diskaudio.c",
        "src/audio/dummy/SDL_dummyaudio.c",
        "src/audio/wasapi/SDL_wasapi.c",
        "src/audio/wasapi/SDL_wasapi_win32.c",
        "src/audio/winmm/SDL_winmm.c",
        "src/core/windows/SDL_windows.c",
        "src/core/windows/SDL_xinput.c",
        "src/cpuinfo/SDL_cpuinfo.c",
        "src/dynapi/SDL_dynapi.c",
        "src/events/SDL_clipboardevents.c",
        "src/events/SDL_displayevents.c",
        "src/events/SDL_dropevents.c",
        "src/events/SDL_events.c",
        "src/events/SDL_gesture.c",
        "src/events/SDL_keyboard.c",
        "src/events/SDL_mouse.c",
        "src/events/SDL_quit.c",
        "src/events/SDL_touch.c",
        "src/events/SDL_windowevents.c",
        "src/file/SDL_rwops.c",
        "src/filesystem/windows/SDL_sysfilesystem.c",
        "src/haptic/SDL_haptic.c",
        "src/haptic/windows/SDL_dinputhaptic.c",
        "src/haptic/windows/SDL_windowshaptic.c",
        "src/haptic/windows/SDL_xinputhaptic.c",
        "src/hidapi/windows/hid.c",
        "src/joystick/SDL_gamecontroller.c",
        "src/joystick/SDL_joystick.c",
        "src/joystick/hidapi/SDL_hidapi_gamecube.c",
        "src/joystick/hidapi/SDL_hidapi_ps4.c",
        "src/joystick/hidapi/SDL_hidapi_rumble.c",
        "src/joystick/hidapi/SDL_hidapi_steam.c",
        "src/joystick/hidapi/SDL_hidapi_switch.c",
        "src/joystick/hidapi/SDL_hidapi_xbox360.c",
        "src/joystick/hidapi/SDL_hidapi_xbox360w.c",
        "src/joystick/hidapi/SDL_hidapi_xboxone.c",
        "src/joystick/hidapi/SDL_hidapijoystick.c",
        "src/joystick/windows/SDL_dinputjoystick.c",
        "src/joystick/windows/SDL_mmjoystick.c",
        "src/joystick/windows/SDL_windowsjoystick.c",
        "src/joystick/windows/SDL_xinputjoystick.c",
        "src/libm/e_atan2.c",
        "src/libm/e_exp.c",
        "src/libm/e_fmod.c",
        "src/libm/e_log.c",
        "src/libm/e_log10.c",
        "src/libm/e_pow.c",
        "src/libm/e_rem_pio2.c",
        "src/libm/e_sqrt.c",
        "src/libm/k_cos.c",
        "src/libm/k_rem_pio2.c",
        "src/libm/k_sin.c",
        "src/libm/k_tan.c",
        "src/libm/s_atan.c",
        "src/libm/s_copysign.c",
        "src/libm/s_cos.c",
        "src/libm/s_fabs.c",
        "src/libm/s_floor.c",
        "src/libm/s_scalbn.c",
        "src/libm/s_sin.c",
        "src/libm/s_tan.c",
        "src/loadso/windows/SDL_sysloadso.c",
        "src/power/SDL_power.c",
        "src/power/windows/SDL_syspower.c",
        "src/render/SDL_d3dmath.c",
        "src/render/SDL_render.c",
        "src/render/SDL_yuv_sw.c",
        "src/render/direct3d/SDL_render_d3d.c",
        "src/render/direct3d/SDL_shaders_d3d.c",
        "src/render/direct3d11/SDL_render_d3d11.c",
        "src/render/direct3d11/SDL_shaders_d3d11.c",
        "src/render/opengl/SDL_render_gl.c",
        "src/render/opengl/SDL_shaders_gl.c",
        "src/render/opengles/SDL_render_gles.c",
        "src/render/opengles2/SDL_render_gles2.c",
        "src/render/opengles2/SDL_shaders_gles2.c",
        "src/render/psp/SDL_render_psp.c",
        "src/render/software/SDL_blendfillrect.c",
        "src/render/software/SDL_blendline.c",
        "src/render/software/SDL_blendpoint.c",
        "src/render/software/SDL_drawline.c",
        "src/render/software/SDL_drawpoint.c",
        "src/render/software/SDL_render_sw.c",
        "src/render/software/SDL_rotate.c",
        "src/sensor/SDL_sensor.c",
        "src/sensor/dummy/SDL_dummysensor.c",
        "src/stdlib/SDL_getenv.c",
        "src/stdlib/SDL_iconv.c",
        "src/stdlib/SDL_malloc.c",
        "src/stdlib/SDL_qsort.c",
        "src/stdlib/SDL_stdlib.c",
        "src/stdlib/SDL_string.c",
        "src/stdlib/SDL_strtokr.c",
        "src/thread/SDL_thread.c",
        "src/thread/generic/SDL_syscond.c",
        "src/thread/windows/SDL_sysmutex.c",
        "src/thread/windows/SDL_syssem.c",
        "src/thread/windows/SDL_systhread.c",
        "src/thread/windows/SDL_systls.c",
        "src/timer/SDL_timer.c",
        "src/timer/windows/SDL_systimer.c",
        "src/video/SDL_RLEaccel.c",
        "src/video/SDL_blit.c",
        "src/video/SDL_blit_0.c",
        "src/video/SDL_blit_1.c",
        "src/video/SDL_blit_A.c",
        "src/video/SDL_blit_N.c",
        "src/video/SDL_blit_auto.c",
        "src/video/SDL_blit_copy.c",
        "src/video/SDL_blit_slow.c",
        "src/video/SDL_bmp.c",
        "src/video/SDL_clipboard.c",
        "src/video/SDL_egl.c",
        "src/video/SDL_fillrect.c",
        "src/video/SDL_pixels.c",
        "src/video/SDL_rect.c",
        "src/video/SDL_shape.c",
        "src/video/SDL_stretch.c",
        "src/video/SDL_surface.c",
        "src/video/SDL_video.c",
        "src/video/SDL_vulkan_utils.c",
        "src/video/SDL_yuv.c",
        "src/video/dummy/SDL_nullevents.c",
        "src/video/dummy/SDL_nullframebuffer.c",
        "src/video/dummy/SDL_nullvideo.c",
        "src/video/windows/SDL_windowsclipboard.c",
        "src/video/windows/SDL_windowsevents.c",
        "src/video/windows/SDL_windowsframebuffer.c",
        "src/video/windows/SDL_windowskeyboard.c",
        "src/video/windows/SDL_windowsmessagebox.c",
        "src/video/windows/SDL_windowsmodes.c",
        "src/video/windows/SDL_windowsmouse.c",
        "src/video/windows/SDL_windowsopengl.c",
        "src/video/windows/SDL_windowsopengles.c",
        "src/video/windows/SDL_windowsshape.c",
        "src/video/windows/SDL_windowsvideo.c",
        "src/video/windows/SDL_windowsvulkan.c",
        "src/video/windows/SDL_windowswindow.c",
        "src/video/yuv2rgb/yuv_rgb.c",
    ],
    "@//:macos": [
        # All macos srcs are defined in the :sdl2_macos target.
    ],
})

_SDL_COPTS = select({
    "@//:windows": [
        "/W3",
        # "/MD",
        # "/O2",
        "/Ob2",
        "/GS-",
        # "/arch:SSE",
    ],
    "@//:macos": [
        "-msse3",
        "-msse2",
        "-msse",
        "-m3dnow",
        "-mmmx",
        "-Wshadow",
        "-fvisibility=hidden",
        "-Wdeclaration-after-statement",
        "-Werror=declaration-after-statement",
        "-fno-strict-aliasing",
        "-Wall",
        # "-O3",
    ],
})

_SDL_DEFINES = select({
    "@//:windows": [
        "WIN32",
        "_WINDOWS",
        "NDEBUG",
    ],
    "@//:macos": [
        "USING_GENERATED_CONFIG_H",
        "_THREAD_SAFE",
        "NDEBUG",
    ],
})

_SDL_INCLUDES = select({
    "@//:windows": [
        "include",
        "src/hidapi/hidapi",
        "src/video/khronos",
    ],
    "@//:macos": [
        "include/macos",
        "include",
        "src/audio/coreaudio",
        "src/filesystem/cocoa",
        "src/thread/pthread",
        "src/video/cocoa",
        "src/video/khronos",
    ],
})

_SDL_LINKOPTS = select({
    "@//:windows": [
        "-DEFAULTLIB:user32",
        "-DEFAULTLIB:gdi32",
        "-DEFAULTLIB:winmm",
        "-DEFAULTLIB:imm32",
        "-DEFAULTLIB:ole32",
        "-DEFAULTLIB:oleaut32",
        "-DEFAULTLIB:version",
        "-DEFAULTLIB:uuid",
        "-DEFAULTLIB:shell32",
        "-DEFAULTLIB:advapi32",
        "-DEFAULTLIB:hid",
        "-DEFAULTLIB:setupapi",
        "-DEFAULTLIB:opengl32",
        "-DEFAULTLIB:kernel32",
    ],
    "@//:macos": [
        "-liconv",
        # "-lm",
    ],
})

_SDL_DEPS = select({
    "@//:windows": [],
    "@//:macos": [
        ":sdl2_macos",
    ],
})

cc_library(
    name = "sdl2_hdrs",
    hdrs = _SDL_HDRS,
    defines = _SDL_DEFINES,
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "sdl2",
    srcs = _SDL_SRC_HDRS + _SDL_SRCS,
    hdrs = _SDL_HDRS,
    copts = _SDL_COPTS,
    defines = _SDL_DEFINES,
    includes = _SDL_INCLUDES,
    linkopts = _SDL_LINKOPTS,
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
    deps = _SDL_DEPS,
)

objc_library(
    name = "sdl2_macos",
    srcs = _SDL_SRC_HDRS + [
        "src/SDL.c",
        "src/SDL_assert.c",
        "src/SDL_dataqueue.c",
        "src/SDL_error.c",
        "src/SDL_hints.c",
        "src/SDL_log.c",
        "src/atomic/SDL_atomic.c",
        "src/atomic/SDL_spinlock.c",
        "src/audio/SDL_audio.c",
        "src/audio/SDL_audiocvt.c",
        "src/audio/SDL_audiodev.c",
        "src/audio/SDL_audiotypecvt.c",
        "src/audio/SDL_mixer.c",
        "src/audio/SDL_wave.c",
        "src/cpuinfo/SDL_cpuinfo.c",
        "src/dynapi/SDL_dynapi.c",
        "src/events/SDL_clipboardevents.c",
        "src/events/SDL_displayevents.c",
        "src/events/SDL_dropevents.c",
        "src/events/SDL_events.c",
        "src/events/SDL_gesture.c",
        "src/events/SDL_keyboard.c",
        "src/events/SDL_mouse.c",
        "src/events/SDL_quit.c",
        "src/events/SDL_touch.c",
        "src/events/SDL_windowevents.c",
        "src/file/SDL_rwops.c",
        "src/libm/e_atan2.c",
        "src/libm/e_exp.c",
        "src/libm/e_fmod.c",
        "src/libm/e_log.c",
        "src/libm/e_log10.c",
        "src/libm/e_pow.c",
        "src/libm/e_rem_pio2.c",
        "src/libm/e_sqrt.c",
        "src/libm/k_cos.c",
        "src/libm/k_rem_pio2.c",
        "src/libm/k_sin.c",
        "src/libm/k_tan.c",
        "src/libm/s_atan.c",
        "src/libm/s_copysign.c",
        "src/libm/s_cos.c",
        "src/libm/s_fabs.c",
        "src/libm/s_floor.c",
        "src/libm/s_scalbn.c",
        "src/libm/s_sin.c",
        "src/libm/s_tan.c",
        "src/render/SDL_d3dmath.c",
        "src/render/SDL_render.c",
        "src/render/SDL_yuv_sw.c",
        "src/render/direct3d/SDL_render_d3d.c",
        "src/render/direct3d/SDL_shaders_d3d.c",
        "src/render/direct3d11/SDL_render_d3d11.c",
        "src/render/direct3d11/SDL_shaders_d3d11.c",
        "src/render/opengl/SDL_render_gl.c",
        "src/render/opengl/SDL_shaders_gl.c",
        "src/render/opengles/SDL_render_gles.c",
        "src/render/opengles2/SDL_render_gles2.c",
        "src/render/opengles2/SDL_shaders_gles2.c",
        "src/render/psp/SDL_render_psp.c",
        "src/render/software/SDL_blendfillrect.c",
        "src/render/software/SDL_blendline.c",
        "src/render/software/SDL_blendpoint.c",
        "src/render/software/SDL_drawline.c",
        "src/render/software/SDL_drawpoint.c",
        "src/render/software/SDL_render_sw.c",
        "src/render/software/SDL_rotate.c",
        "src/stdlib/SDL_getenv.c",
        "src/stdlib/SDL_iconv.c",
        "src/stdlib/SDL_malloc.c",
        "src/stdlib/SDL_qsort.c",
        "src/stdlib/SDL_stdlib.c",
        "src/stdlib/SDL_string.c",
        "src/stdlib/SDL_strtokr.c",
        "src/thread/SDL_thread.c",
        "src/timer/SDL_timer.c",
        "src/video/SDL_RLEaccel.c",
        "src/video/SDL_blit.c",
        "src/video/SDL_blit_0.c",
        "src/video/SDL_blit_1.c",
        "src/video/SDL_blit_A.c",
        "src/video/SDL_blit_N.c",
        "src/video/SDL_blit_auto.c",
        "src/video/SDL_blit_copy.c",
        "src/video/SDL_blit_slow.c",
        "src/video/SDL_bmp.c",
        "src/video/SDL_clipboard.c",
        "src/video/SDL_egl.c",
        "src/video/SDL_fillrect.c",
        "src/video/SDL_pixels.c",
        "src/video/SDL_rect.c",
        "src/video/SDL_shape.c",
        "src/video/SDL_stretch.c",
        "src/video/SDL_surface.c",
        "src/video/SDL_video.c",
        "src/video/SDL_vulkan_utils.c",
        "src/video/SDL_yuv.c",
        "src/video/yuv2rgb/yuv_rgb.c",
        "src/joystick/SDL_gamecontroller.c",
        "src/joystick/SDL_joystick.c",
        "src/haptic/SDL_haptic.c",
        "src/sensor/SDL_sensor.c",
        "src/power/SDL_power.c",
        "src/audio/dummy/SDL_dummyaudio.c",
        "src/audio/disk/SDL_diskaudio.c",
        "src/loadso/dlopen/SDL_sysloadso.c",
        "src/video/dummy/SDL_nullevents.c",
        "src/video/dummy/SDL_nullframebuffer.c",
        "src/video/dummy/SDL_nullvideo.c",
        "src/joystick/darwin/SDL_sysjoystick.c",
        "src/haptic/darwin/SDL_syshaptic.c",
        "src/power/macosx/SDL_syspower.c",
        "src/timer/unix/SDL_systimer.c",
        "src/thread/pthread/SDL_systhread.c",
        "src/thread/pthread/SDL_sysmutex.c",
        "src/thread/pthread/SDL_syscond.c",
        "src/thread/pthread/SDL_systls.c",
        "src/thread/pthread/SDL_syssem.c",
        "src/sensor/dummy/SDL_dummysensor.c",
    ],
    hdrs = _SDL_HDRS,
    includes = _SDL_INCLUDES,
    non_arc_srcs = [
        "src/file/cocoa/SDL_rwopsbundlesupport.m",
        "src/audio/coreaudio/SDL_coreaudio.m",
        "src/filesystem/cocoa/SDL_sysfilesystem.m",
        "src/video/cocoa/SDL_cocoaclipboard.m",
        "src/video/cocoa/SDL_cocoaevents.m",
        "src/video/cocoa/SDL_cocoakeyboard.m",
        "src/video/cocoa/SDL_cocoamessagebox.m",
        "src/video/cocoa/SDL_cocoametalview.m",
        "src/video/cocoa/SDL_cocoamodes.m",
        "src/video/cocoa/SDL_cocoamouse.m",
        "src/video/cocoa/SDL_cocoamousetap.m",
        "src/video/cocoa/SDL_cocoaopengl.m",
        "src/video/cocoa/SDL_cocoaopengles.m",
        "src/video/cocoa/SDL_cocoashape.m",
        "src/video/cocoa/SDL_cocoavideo.m",
        "src/video/cocoa/SDL_cocoavulkan.m",
        "src/video/cocoa/SDL_cocoawindow.m",
    ],
    sdk_frameworks = [
        "Foundation",
        "AVFoundation",
        "AudioToolbox",
        "CoreAudio",
        "Carbon",
        "ForceFeedback",
        "IOKit",
        "Cocoa",
        "CoreVideo",
    ],
    textual_hdrs = [
        "src/thread/generic/SDL_syssem.c",
    ],
)

copy_file(
    name = "sdl2_config",
    src = select({
        "@//:windows": "@//third_party/external:sdl_config.default.h",
        "@//:macos": "@//third_party/external:sdl_config.macos.h",
    }),
    out = "include/SDL_config.h",
)

_SDL_MAIN_SRCS = select({
    "@//:windows": [
        "src/main/windows/SDL_windows_main.c",
    ],
    "@//:macos": [
        "src/main/dummy/SDL_dummy_main.c",
    ],
})

cc_library(
    name = "sdl2_main",
    srcs = _SDL_SRC_HDRS + _SDL_MAIN_SRCS,
    hdrs = _SDL_HDRS,
    copts = _SDL_COPTS,
    defines = _SDL_DEFINES,
    includes = _SDL_INCLUDES,
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
)
