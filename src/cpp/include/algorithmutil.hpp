#ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_
#define HEADER_SCRIPTS_CXX_AUTO_TIMER_

#include <algorithm>
#include <functional>
#include <ranges>

namespace xph {
    // template <std::ranges::range Tit, class Top>
    //    inline constexpr auto transform(Tit& iterable, Top operation);
    //
    //    template <std::ranges::range Tit, class Top>
    //    inline constexpr auto transform(Tit& iterable, Top operation)
    //    {
    //        return std::transform(iterable.cbegin(), iterable.cend(), iterable.begin(), operation);
    //    }

    template <std::ranges::range Tit, class Tfin>
    inline constexpr auto transform(Tit& iterable,
                                    std::function<std::ranges::range_value_t<Tit>(Tfin)> func);

    template <std::ranges::range Tit, class Tfin>
    inline constexpr auto transform(Tit& iterable,
                                    std::function<std::ranges::range_value_t<Tit>(Tfin)> func)
    {
        using Tfout = std::ranges::range_value_t<Tit>;
        return std::transform(iterable.cbegin(),
                              iterable.cend(),
                              iterable.begin(),
                              static_cast<Tfout>([&](Tfin e) { return func(e); }));
    }

    template <std::ranges::range Tit, class Tfin>
    inline constexpr auto transform(Tit& iterable, std::ranges::range_value_t<Tit> (*func)(Tfin))
    {
        using Tfout = std::ranges::range_value_t<Tit>;
        return std::transform(iterable.cbegin(),
                              iterable.cend(),
                              iterable.begin(),
                              static_cast<Tfout>([&](Tfin e) { return func(e); }));
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_ */
