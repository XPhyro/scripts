#ifndef HEADER_SCRIPTS_CXX_VECUTIL_
#define HEADER_SCRIPTS_CXX_VECUTIL_

#include <cstdint>
#include <string>

namespace xph::unit {
    inline std::string tick_to_human(std::size_t tick)
    {
        enum { unit_count = 10 };

        static const std::string units[unit_count] = {
            "y", "mo", "w", "d", "h", "m", "s", "ms", "us", "ns",
        };
        static const std::size_t durations[unit_count] = {
            31'556'952'000'000'000uz,
            2'629'746'000'000'000uz,
            604'800'000'000'000uz,
            86'400'000'000'000uz,
            3'600'000'000'000uz,
            60'000'000'000uz,
            1'000'000'000uz,
            1'000'000uz,
            1'000uz,
            1uz,
        };

        if (!tick)
            return "0s";

        std::string s = "";
        for (int i = 0; i < unit_count; ++i) {
            if (tick >= durations[i]) {
                s += std::to_string(tick / durations[i]) + units[i];
                tick %= durations[i];
            }
        }

        return s;
    }
} // namespace xph::unit

#endif /* ifndef HEADER_SCRIPTS_CXX_VECUTIL_ */
