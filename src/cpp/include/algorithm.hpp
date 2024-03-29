#ifndef HEADER_SCRIPTS_CXX_ALGORITHM_
#define HEADER_SCRIPTS_CXX_ALGORITHM_

#include <algorithm>

namespace xph {
    template <std::ranges::range Tit, class Top>
    inline constexpr auto transform(Tit& iterable, Top operation);

    template <std::ranges::range Tit, class Top>
    inline constexpr auto transform(Tit& iterable, Top operation)
    {
        return std::transform(iterable.cbegin(), iterable.cend(), iterable.begin(), operation);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_ALGORITHM_ */
