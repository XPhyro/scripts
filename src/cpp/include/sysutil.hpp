#ifndef HEADER_SCRIPTS_CXX_SYSUTIL
#define HEADER_SCRIPTS_CXX_SYSUTIL

#include <filesystem>
#include <string>

namespace sysutil
{
void swapfile(std::string& path1, std::string& path2)
{
    auto pathtmp = std::tmpnam(nullptr);
    std::filesystem::rename(path1, pathtmp);
    std::filesystem::rename(path2, path1);
    std::filesystem::rename(pathtmp, path2);
    std::filesystem::remove(pathtmp);
}
} // namespace sysutil

#endif /* ifndef HEADER_SCRIPTS_CXX_SYSUTIL */
