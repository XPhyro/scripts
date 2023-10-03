#ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_
#define HEADER_SCRIPTS_CXX_AUTO_TIMER_

#include <algorithm>
#include <functional>
#include <ranges>

namespace xph {
    template <std::ranges::range Tit, class Top>
    inline constexpr auto transform_in_place(Tit& iterable, Top operation);

    template <std::ranges::range Tit, class Top>
    inline constexpr auto transform_in_place(Tit& iterable, Top operation)
    {
        return std::transform(iterable.cbegin(), iterable.cend(), iterable.begin(), operation);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_ */