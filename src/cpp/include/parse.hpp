#ifndef HEADER_SCRIPTS_CXX_CASTUTIL
#define HEADER_SCRIPTS_CXX_CASTUTIL

#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <consts.hpp>

namespace xph
{
template <typename T>
[[gnu::always_inline, nodiscard]] inline T _parse(const std::string_view& s)
{
    static std::stringstream ss;

    ss.str(xph::consts::str::empty);
    ss.clear();

    ss << s;

    T t;
    ss >> t;

    if (ss.fail())
        throw std::runtime_error(
            "given string is not parseable to type T"); // TODO: replace with format_error when <format> is available>

    return std::move(t);
}

template <typename T>
[[nodiscard]] T parse(const std::string_view& s)
{
    return std::move(_parse<T>(s));
}

template <typename T>
[[nodiscard]] T parse(const std::string_view&& s)
{
    return std::move(_parse<T>(s));
}
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_CASTUTIL */