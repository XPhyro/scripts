flags = [
    "-O3",
    "-std=c99",
    "-pedantic",
    "-Wall",
]

SOURCE_EXTENSIONS = [
    ".c",
    ".h",
]


def FlagsForFile(filename, **kwargs):
    return {"flags": flags, "do_cache": True}


def Settings(**kwargs):
    filename = kwargs.pop("filename")
    return FlagsForFile(filename, **kwargs)
