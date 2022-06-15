flags =
    [
        "-O3",
        "-std=c99",
        "-pedantic",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Wno-unused-parameter",
        "-Wno-implicit-fallthrough",
        "-Wno-sign-compare",
        "-Wfloat-equal",
        "-Wdouble-promotion",
        "-Wjump-misses-init",
        "-Wold-style-definition",
        "-Winline",
        "-Wpadded",
        "-Wpacked",
        "-Wdisabled-optimization",
        "-lm",
    ]

    SOURCE_EXTENSIONS =
        [
            ".c",
            ".h",
        ]

    def
    FlagsForFile(filename, **kwargs)
    : return { "flags": flags, "do_cache": True }

    def Settings(**kwargs)
    : filename = kwargs.pop("filename") return FlagsForFile(filename, **kwargs)
