import os
import ycm_core


flags = [
    "-O3",
    "-std=c++20",  # YCM does not support c++23 yet
    "-Wall",
]

SOURCE_EXTENSIONS = [
    ".cpp",
]


def FlagsForFile(filename, **kwargs):
    return {"flags": flags, "do_cache": True}
