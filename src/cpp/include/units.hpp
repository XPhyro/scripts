#ifndef HEADER_SCRIPTS_CXX_UNITS_
#define HEADER_SCRIPTS_CXX_UNITS_

#include <array>
#include <string>

namespace xph::unit {
    inline std::string tick_to_human(std::size_t tick)
    {
        static const std::array<std::string, 10> units = {
            "y", "mo", "w", "d", "h", "m", "s", "ms", "us", "ns",
        };
        static const std::array<std::size_t, units.size()> durations = {
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
        for (auto i = 0uz; i < units.size(); ++i) {
            if (tick >= durations[i]) {
                s += std::to_string(tick / durations[i]) + units[i];
                tick %= durations[i];
            }
        }

        return s;
    }
} // namespace xph::unit

#endif /* ifndef HEADER_SCRIPTS_CXX_UNITS_ */
