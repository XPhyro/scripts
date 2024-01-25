#ifndef HEADER_SCRIPTS_CXX_LINALG_
#define HEADER_SCRIPTS_CXX_LINALG_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <ranges>
#include <span>
#include <vector>

#include "algorithm.hpp"
#include "math.hpp"

namespace xph::linalg {
    // None to Many

    template <typename Tdata, typename Tarr>
    inline void arange(Tdata start, Tdata stop, Tdata step, Tarr& arr)
    {
        arr.reserve(arr.size() + (stop - start) / step);
        for (auto current = start; current < stop; current += step)
            arr.push_back(current);
    }

    template <typename Tdata>
    inline std::vector<Tdata> arange(Tdata start, Tdata stop, Tdata step)
    {
        std::vector<Tdata> container({}, (stop - start) / step);
        arange(start, stop, step, container);
        return container;
    }

    // One to Many

    template <typename Tarr>
    void factor(Tarr& arr)
    {
        Tarr factors;

        for (std::intmax_t num : arr) {
            if (!num)
                continue;

            if (num < 0)
                num = -num;

            while (!(num % 2)) {
                num /= 2;
                factors.push_back(2);
            }

            for (auto fac = 3; fac <= num; fac += 2) {
                while (!(num % fac)) {
                    num /= fac;
                    factors.push_back(fac);
                }
            }
        }

        arr.swap(factors);
    }

    // Many to Many

    template <typename Tarr>
    inline void difference(Tarr& arr)
    {
        // TODO: remove version check after gcc 14 is officially released
        // TODO: add pure ranges implementations for the other functions too
#if __GNUC__ >= 14
        Tarr difference = arr | std::views::slide(2) |
                          std::views::transform([](auto window) { return window[1] - window[0]; }) |
                          std::ranges::to<std::vector>();
#else
        Tarr difference;

        difference.reserve(arr.size() - 1);
        std::ranges::copy(arr | std::views::slide(2) | std::views::transform([](auto window) {
                              return window[1] - window[0];
                          }),
                          std::back_inserter(difference));
#endif

        arr.swap(difference);
    }

    template <typename Tarr>
    inline void partsum(Tarr& arr, std::size_t window_size)
    {
        Tarr partsum;

        partsum.reserve(arr.size() - window_size + 1);
        std::ranges::copy(arr | std::views::slide(window_size) |
                              std::views::transform([](auto window) {
                                  return std::accumulate(window.begin(), window.end(), 0.0);
                              }),
                          std::back_inserter(partsum));

        arr.swap(partsum);
    }

    template <typename Tarr>
    inline void partaltsum(Tarr& arr, std::size_t window_size)
    {
        Tarr partaltsum;

        partaltsum.reserve(arr.size() - window_size + 1);
        std::ranges::copy(
            arr | std::views::slide(window_size) | std::views::transform([](auto window) {
                return std::accumulate(
                    window.begin(), window.end(), 0.0, [&](double cumaltsum, double num) {
                        static double sign = -1.0;
                        return cumaltsum + num * (sign = -sign);
                    });
            }),
            std::back_inserter(partaltsum));

        arr.swap(partaltsum);
    }

    template <typename Tarr>
    inline void partprod(Tarr& arr, std::size_t window_size)
    {
        Tarr partprod;

        partprod.reserve(arr.size() - window_size + 1);
        std::ranges::copy(
            arr | std::views::slide(window_size) | std::views::transform([](auto window) {
                return std::accumulate(window.begin(), window.end(), 1.0, std::multiplies<>());
            }),
            std::back_inserter(partprod));

        arr.swap(partprod);
    }

    // One to One

    template <typename Tdata>
    inline void cumsum(std::span<Tdata> arr)
    {
        std::partial_sum(arr.begin(), arr.end(), arr.begin());
    }

    template <typename Tdata>
    inline void cumaltsum(std::span<Tdata> arr)
    {
        static_assert(Tdata(-1.0) < 0);
        Tdata cumaltsum = 0.0;
        Tdata sign = -1.0;
        for (Tdata& val : arr) {
            // cppcheck-suppress useStlAlgorithm
            val = cumaltsum += val * (sign = -sign);
        }
    }

    template <typename Tdata>
    inline void cumprod(std::span<Tdata> arr)
    {
        std::partial_sum(arr.begin(), arr.end(), arr.begin(), [](const Tdata& x, const Tdata& y) {
            return x * y;
        });
    }

    template <typename Tdata>
    void factorial(std::span<Tdata> arr)
    {
#if false // no gcc feature-test for fold_left
        xph::transform(arr, [](Tdata num) {
            return std::ranges::fold_left(std::views::iota(1ull, static_cast<std::uintmax_t>(num) + 1ull),
                                          std::multiplies<>());
        });
#endif
        class factorial_impl final {
        public:
            static inline std::uintmax_t factorial(std::uintmax_t num)
            {
                auto range = std::views::iota(1ull, num + 1ull);
                return std::accumulate(range.begin(), range.end(), 1ull, std::multiplies<>());
            }
        };

        xph::transform(arr,
                       [](Tdata num) { return factorial_impl::factorial(num >= 0 ? num : -num); });
    }

    template <typename Tdata>
    inline void degrees(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return val * Tdata{ 180.0 / std::numbers::pi }; });
    }

    template <typename Tdata>
    inline void radians(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return val * Tdata{ std::numbers::pi / 180.0 }; });
    }

    template <typename Tdata>
    inline void acos(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::acos(val); });
    }

    template <typename Tdata>
    inline void asin(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::asin(val); });
    }

    template <typename Tdata>
    inline void atan(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::atan(val); });
    }

    template <typename Tdata>
    inline void cbrt(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::cbrt(val); });
    }

    template <typename Tdata>
    inline void ceil(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::ceil(val); });
    }

    template <typename Tdata>
    inline void cos(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::cos(val); });
    }

    template <typename Tdata>
    inline void cosh(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::cosh(val); });
    }

    template <typename Tdata>
    inline void exp(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::exp(val); });
    }

    template <typename Tdata>
    inline void abs(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::fabs(val); });
    }

    template <typename Tdata>
    inline void floor(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::floor(val); });
    }

    template <typename Tdata>
    inline void log(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::log(val); });
    }

    template <typename Tdata>
    inline void log10(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::log10(val); });
    }

    template <typename Tdata>
    inline void log2(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::log2(val); });
    }

    template <typename Tdata>
    inline void log1p(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::log(Tdata{ 1.0 } + val); });
    }

    template <typename Tdata, typename Texponent>
    inline void pow(std::span<Tdata> arr, Texponent exponent)
    {
        xph::transform(arr, [&](Tdata val) { return std::pow(val, exponent); });
    }

    template <typename Tdata>
    inline void round(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::round(val); });
    }

    template <typename Tdata>
    inline void sin(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::sin(val); });
    }

    template <typename Tdata>
    inline void sinh(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::sinh(val); });
    }

    template <typename Tdata>
    inline void sqrt(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::sqrt(val); });
    }

    template <typename Tdata>
    inline void tan(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::tan(val); });
    }

    template <typename Tdata>
    inline void tanh(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::tanh(val); });
    }

    template <typename Tdata>
    inline void trunc(std::span<Tdata> arr)
    {
        xph::transform(arr, [](Tdata val) { return std::trunc(val); });
    }

    // One to Optional One

    template <typename Tarr>
    void zero(Tarr& arr)
    {
        std::erase_if(arr, [](auto val) { return !xph::approx_zero(val); });
    }

    template <typename Tarr>
    void nonzero(Tarr& arr)
    {
        std::erase_if(arr, [](auto val) { return xph::approx_zero(val); });
    }

    // Many to One

    template <typename Tarr>
    void count(Tarr& arr)
    {
        arr = { arr.size() };
    }

    template <typename Tarr>
    void max(Tarr& arr)
    {
        arr = { *std::max_element(arr.begin(), arr.end()) };
    }

    template <typename Tarr>
    void min(Tarr& arr)
    {
        arr = { *std::min_element(arr.begin(), arr.end()) };
    }

    template <typename Tarr>
    void sum(Tarr& arr)
    {
        arr = { std::accumulate(arr.begin(), arr.end(), 0.0) };
    }

    template <typename Tarr>
    void altsum(Tarr& arr)
    {
        static_assert(typename Tarr::value_type(-1.0) < 0);
        arr = { std::accumulate(arr.begin(), arr.end(), 0.0, [&](auto altsum, auto val) {
            static decltype(val) sign = -1.0;
            return altsum + val * (sign = -sign);
        }) };
    }

    template <typename Tarr>
    void product(Tarr& arr)
    {
        arr = { std::accumulate(arr.begin(), arr.end(), 1.0, std::multiplies<>()) };
    }

    template <typename Tarr>
    void mean(Tarr& arr)
    {
        arr = { std::accumulate(arr.begin(), arr.end(), 0.0) / arr.size() };
    }

    template <typename Tarr>
    void std(Tarr& arr)
    {
        const auto mean = std::accumulate(arr.begin(), arr.end(), 0.0) / arr.size();
        arr = { std::sqrt(std::accumulate(arr.begin(),
                                          arr.end(),
                                          0.0,
                                          [&](auto accum, auto val) {
                                              auto diff = val - mean;
                                              return accum + diff * diff;
                                          }) /
                          (arr.size() - 1)) };
    }

    template <typename Tarr>
    void median(Tarr& arr)
    {
        std::sort(arr.begin(), arr.end());
        arr = { arr.size() % 2 ? arr[arr.size() / 2] :
                                 (arr[arr.size() / 2 - 1] + arr[arr.size() / 2]) / 2 };
    }

    template <typename Tarr>
    void gmean(Tarr& arr)
    {
        arr = { std::pow(
            std::accumulate(
                arr.begin(), arr.end(), 1.0, [&](auto accum, auto num) { return accum * num; }),
            1.0 / arr.size()) };
    }

    template <typename Tarr>
    void hmean(Tarr& arr)
    {
        arr = { arr.size() /
                std::accumulate(arr.begin(), arr.end(), 0.0, [&](auto accum, auto num) {
                    return accum + 1.0 / num;
                }) };
    }

    template <typename Tarr>
    void gcd(Tarr& arr)
    {
        auto min = std::numeric_limits<typename Tarr::value_type>::max();
        auto max = std::numeric_limits<typename Tarr::value_type>::min();

        for (auto num : arr) {
            num = std::abs(num);
            if (num < min)
                min = num;
            if (num > max)
                max = num;
        }

        class gcd_impl final {
        public:
            static inline std::uintmax_t gcd_pair(std::uintmax_t min, std::uintmax_t max)
            {
                return !max ? min : gcd_pair(max, min % max);
            }
        };

        arr = { gcd_impl::gcd_pair(std::abs(min), std::abs(max)) };
    }
} // namespace xph::linalg

#endif /* ifndef HEADER_SCRIPTS_CXX_LINALG_ */
