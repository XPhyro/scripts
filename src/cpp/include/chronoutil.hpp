#ifndef HEADER_SCRIPTS_CXX_CHRONOUTIL_
#define HEADER_SCRIPTS_CXX_CHRONOUTIL_

#include <chrono>
#include <cstdint>
#include <limits>

#include <hedley.h>

namespace xph::chrono {
    template <typename T>
    std::chrono::duration<std::int64_t>
    duration_cast_auto(const std::chrono::duration<T>& duration);

    template <typename T>
    std::chrono::duration<std::int64_t> inline duration_cast_auto(
        const std::chrono::duration<T>& duration)
    {
#define XPH_CHRONO_GET_AUTO_PREFIX_IMPL_COMP <
#define XPH_CHRONO_GET_AUTO_PREFIX_IMPL(UNIT, THRESHOLD)             \
    if (auto d = std::chrono::duration_cast<UNIT>(duration);         \
        d.count() XPH_CHRONO_GET_AUTO_PREFIX_IMPL_COMP(THRESHOLD)) { \
        return d;                                                    \
    }                                                                \
    do {                                                             \
    } while (false)

        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::nanoseconds, 1000);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::microseconds, 1000);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::milliseconds, 1000);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::seconds, 60);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::minutes, 60);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::hours, 24);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::days, 7);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::weeks, 4);
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::months, 12);
#undef XPH_CHRONO_GET_AUTO_PREFIX_IMPL_COMP
#define XPH_CHRONO_GET_AUTO_PREFIX_IMPL_COMP <=
        XPH_CHRONO_GET_AUTO_PREFIX_IMPL(std::chrono::years,
                                        std::numeric_limits<std::int64_t>::max());

        HEDLEY_UNREACHABLE();
        return duration;
    }
} // namespace xph::chrono

#endif /* ifndef HEADER_SCRIPTS_CXX_CHRONOUTIL_ */
