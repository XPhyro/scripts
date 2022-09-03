#ifndef HEADER_SCRIPTS_CXX_STRUTIL
#define HEADER_SCRIPTS_CXX_STRUTIL

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

#include <consts.hpp>

namespace strutil
{
std::unique_ptr<std::string> getlower(const std::string str)
{
    auto dup = new std::string(str);
    std::transform(dup->begin(), dup->end(), dup->begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return std::make_unique<std::string>(*dup);
}

std::string constexpr makelower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}

auto constexpr splitview(char delim)
{
    return std::views::split(delim) | std::views::transform([](const auto&& r) -> std::string {
               return { &*r.begin(),
                        static_cast<std::string::size_type>(std::ranges::distance(r)) };
           });
}

std::vector<std::string> constexpr split(const std::string& str, char delim, bool ignoreend = true)
{
    std::vector<std::string> tokens;

    for (auto&& token : str | splitview(delim))
        tokens.push_back(token);

    // TODO: could use `| std::views::drop_last(static_cast<int>(ignoreend && *str.end() == delim))` with C++23
    if (ignoreend && *str.end() == delim)
        tokens.pop_back();

    return std::move(tokens);
}

std::vector<std::string> constexpr split(std::vector<std::string>& tokens, const std::string& str,
                                         char delim, bool ignoreend = true)
{
    auto initsize = tokens.size();

    for (auto&& token : str | splitview(delim))
        tokens.push_back(token);

    // TODO: could use `| std::views::drop_last(static_cast<int>(ignoreend && *str.end() == delim))` with C++23
    if (ignoreend && str.back() == delim && tokens.size() != initsize)
        tokens.pop_back();

    return tokens;
}

constexpr void replaceall(std::string& str, const std::string&& from, const std::string&& to)
{
    std::string newstr;
    newstr.reserve(str.length());

    std::string::size_type pos;
    std::string::size_type lastpos = 0;
    while ((pos = str.find(from, lastpos)) != std::string::npos) {
        newstr.append(str, lastpos, pos - lastpos);
        newstr += to;
        lastpos = pos + from.length();
    }

    newstr.append(str, lastpos, str.length() - lastpos);

    str.swap(newstr);
}

constexpr uint32_t crc32(const std::string_view& str)
{
    uint32_t crc = ~0;
    for (auto&& c : str)
        crc = (crc >> 8) ^ consts::zlib_crc_table[(crc ^ c) & ~0];
    return crc ^ ~0;
}
} // namespace strutil

#endif /* ifndef HEADER_SCRIPTS_CXX_STRUTIL */
