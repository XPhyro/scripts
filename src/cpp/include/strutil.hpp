#ifndef HEADER_SCRIPTS_STRUTIL
#define HEADER_SCRIPTS_STRUTIL

#include <algorithm>
#include <memory>
#include <string>

namespace strutil
{
std::unique_ptr<std::string> getlower(const std::string str)
{
    auto dup = new std::string(str);
    std::transform(dup->begin(), dup->end(), dup->begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return std::make_unique<std::string>(*dup);
}

std::string makelower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}
} // namespace strutil

#endif /* ifndef HEADER_SCRIPTS_STRUTIL */
