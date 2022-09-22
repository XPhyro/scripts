#ifndef HEADER_SCRIPTS_CXX_SYSUTIL
#define HEADER_SCRIPTS_CXX_SYSUTIL

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <set>
#include <string>

#include <unistd.h>

#include <strutil.h>

namespace xph::sys
{
void swapfile(const std::filesystem::path& path1, const std::filesystem::path& path2);

inline void swapfile(const std::filesystem::path& path1, const std::filesystem::path& path2)
{
    auto tmppath = astrdup((path1.string() + "XXXXXX").c_str());
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

void swapfile(const std::string& path1, const std::string& path2);

inline void swapfile(const std::string& path1, const std::string& path2)
{
    swapfile(static_cast<std::filesystem::path>(path1), static_cast<std::filesystem::path>(path2));
}

void signals(std::set<int> signals, void (*func)(int)) noexcept;

inline void signals(std::set<int> signals, void (*func)(int)) noexcept
{
    for (auto&& sig : signals)
        signal(sig, func);
}
} // namespace xph::sys

#endif /* ifndef HEADER_SCRIPTS_CXX_SYSUTIL */
