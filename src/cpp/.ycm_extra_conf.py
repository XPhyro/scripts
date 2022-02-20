flags = [
    "-O3",
    "-std=c++20",  # YCM (or some other component) does not support c++23 yet and c++2b does not work
    "-Wall",
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
