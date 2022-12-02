#ifndef HEADER_SCRIPTS_CXX_STRUTIL_
#define HEADER_SCRIPTS_CXX_STRUTIL_

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <functional>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#if __cplusplus >= 202002L
#include <ranges>
#endif // if __cplusplus >= 202002L

#include <consts.hpp>

namespace xph::str {
    std::unique_ptr<std::string> getlower(const std::string& str);

    std::unique_ptr<std::string> inline getlower(const std::string& str)
    {
        auto dup = std::unique_ptr<std::string>{ new std::string(str) };
        std::transform(dup->begin(), dup->end(), dup->begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return dup;
    }

#if __cplusplus >= 202002L
    constexpr std::string makelower(std::string str);

    constexpr std::string inline makelower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return str;
    }

    constexpr auto splitview(char delim);

    constexpr inline auto splitview(char delim)
    {
        return std::views::split(delim) | std::views::transform([](const auto&& r) -> std::string {
                   return { &*r.begin(),
                            static_cast<std::string::size_type>(std::ranges::distance(r)) };
               });
    }

    constexpr inline std::vector<std::string>
    split(const std::string& str, char delim, bool ignoreend = true);

    constexpr std::vector<std::string> split(const std::string& str, char delim, bool ignoreend)
    {
        std::vector<std::string> tokens;

        std::ranges::copy(str | splitview(delim), std::back_inserter(tokens));
        if (ignoreend && str.back() == delim)
            tokens.pop_back();

        return tokens;
    }

    constexpr std::vector<std::string> split(std::vector<std::string>& tokens,
                                             const std::string& str,
                                             char delim,
                                             bool ignoreend = true);

    constexpr inline std::vector<std::string>
    split(std::vector<std::string>& tokens, const std::string& str, char delim, bool ignoreend)
    {
        auto initsize = tokens.size();

        std::ranges::copy(str | splitview(delim), std::back_inserter(tokens));
        if (ignoreend && str.back() == delim && tokens.size() != initsize)
            tokens.pop_back();

        return tokens;
    }

    constexpr void replaceall(std::string& str, const std::string&& from, const std::string&& to);

    constexpr inline void
    replaceall(std::string& str, const std::string&& from, const std::string&& to)
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

    bool starts_with(const std::string& str, const std::string& prefix);

    inline bool starts_with(const std::string& str, const std::string& prefix)
    {
        return str.starts_with(prefix);
    }
#else // if __cplusplus >= 202002L
    bool starts_with(const std::string& str, const std::string& prefix);

    inline bool starts_with(const std::string& str, const std::string& prefix)
    {
        return str.compare(0, prefix.length(), prefix) == 0;
    }
#endif // #else // if __cplusplus >= 202002L

    constexpr uint32_t crc32(const std::string_view& str);

    constexpr inline uint32_t crc32(const std::string_view& str)
    {
        uint32_t crc = ~0;
        for (auto&& c : str)
            crc = (crc >> 8) ^ consts::zlib_crc_table[(crc ^ c) & ~0];
        return crc ^ ~0;
    }

    std::string hash(const std::string& str);

    inline std::string hash(const std::string& str)
    {
        auto hash = std::hash<std::string>{}(str);
        std::bitset<sizeof(hash) * 8> bits(hash);
        static_assert(sizeof(hash) <= sizeof(unsigned long));

        std::stringstream ss;
        ss << std::hex << bits.to_ulong();

        std::string hashstr;
        ss >> hashstr;

        return hashstr;
    }

    void hash_in_place(std::string& str);

    inline void hash_in_place(std::string& str)
    {
        auto hash = std::hash<std::string>{}(str);
        std::bitset<sizeof(hash) * 8> bits(hash);
        static_assert(sizeof(hash) <= sizeof(unsigned long));

        std::stringstream ss;
        ss << std::hex << bits.to_ulong();

        ss >> str;
    }
} // namespace xph::str

#endif /* ifndef HEADER_SCRIPTS_CXX_STRUTIL_ */
