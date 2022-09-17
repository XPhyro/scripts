flags = [
    "-O3",
    "-std=c++2b",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-Wabi=11",
    "-Wswitch-default",
    "-Wimplicit-fallthrough=5",
    "-Wstringop-overflow=4",
    "-Wfloat-equal",
    "-Wdouble-promotion",
    "-Wdisabled-optimization",
    "-Wstrict-null-sentinel",
    "-Wold-style-cast",
    "-Wsign-promo",
    "-fpermissive",
    "-Iinclude",
    "-I../c/include",
    "-I../../lib/hedley",
    "-I../../lib/pstreams",
]

SOURCE_EXTENSIONS = [
    ".cpp",
    ".hpp",
]


def FlagsForFile(filename, **kwargs):
    return {"flags": flags, "do_cache": True}


def Settings(**kwargs):
    filename = kwargs.pop("filename")
    return FlagsForFile(filename, **kwargs)
