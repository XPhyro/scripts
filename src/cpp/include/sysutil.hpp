#ifndef HEADER_SCRIPTS_CXX_SYSUTIL
#define HEADER_SCRIPTS_CXX_SYSUTIL

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

#include <system_error>
#include <unistd.h>

#include <strutil.h>

namespace xph::sys
{
void swapfile(std::string& path1, std::string& path2)
{
    auto tmppath = astrdup((path1 + "XXXXXX").c_str());
    auto tmpfd = mkstemp(tmppath);

    if (tmpfd == -1)
        throw std::filesystem::filesystem_error("could not create temporary file",
                                                tmppath,
                                                std::error_code(errno, std::generic_category()));

    std::filesystem::rename(path1, tmppath);
    std::filesystem::rename(path2, path1);
    std::filesystem::rename(tmppath, path2);
    std::filesystem::remove(tmppath);

    close(tmpfd);
    free(tmppath);
}
} // namespace xph::sys

#endif /* ifndef HEADER_SCRIPTS_CXX_SYSUTIL */
