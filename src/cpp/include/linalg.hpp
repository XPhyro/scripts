#ifndef HEADER_SCRIPTS_CXX_LINALG_
#define HEADER_SCRIPTS_CXX_LINALG_

#include <cmath>
#include <cstddef>
#include <numeric>
#include <span>
#include <vector>

#include "algorithm.hpp"

// TODO: Tdata must be numeric

namespace xph::linalg {
    // None to Many

    template <typename Tdata, typename Tcontainer>
    inline void arange(Tdata start, Tdata stop, Tdata step, Tcontainer& container)
    {
        container.reserve(container.size() + (stop - start) / step);
        for (auto current = start; current < stop; current += step)
            container.push_back(current);
    }

    template <typename Tdata>
    inline std::vector<Tdata> arange(Tdata start, Tdata stop, Tdata step)
    {
        std::vector<Tdata> container({}, (stop - start) / step);
        arange(start, stop, step, container);
        return container;
    }

    // One to Many

    // TODO:

    // Many to Many

    template <typename Tcontainer>
    inline void difference(Tcontainer& arr)
    {
        Tcontainer difference;

        difference.reserve(arr.size() - 1);
        for (auto&& window : arr | std::views::slide(2))
            difference.push_back(window[1] - window[0]);

        arr.swap(difference);
    }

    template <typename Tcontainer>
    inline void partsum(Tcontainer& arr, std::size_t window_size)
    {
        Tcontainer partsum;

        partsum.reserve(arr.size() - window_size + 1);
        for (const auto& window : arr | std::views::slide(window_size))
            partsum.push_back(std::accumulate(window.begin(), window.end(), 0.0));

        arr.swap(partsum);
    }

    template <typename Tcontainer>
    inline void partaltsum(Tcontainer& arr, std::size_t window_size)
    {
        Tcontainer partaltsum;

        partaltsum.reserve(arr.size() - window_size + 1);
        for (const auto& window : arr | std::views::slide(window_size)) {
            partaltsum.push_back(std::accumulate(
                window.begin(), window.end(), 0.0, [&](double cumaltsum, double num) {
                    static double sign = -1.0;
                    return cumaltsum + num * (sign = -sign);
                }));
        }

        arr.swap(partaltsum);
    }

    template <typename Tcontainer>
    inline void partprod(Tcontainer& arr, std::size_t window_size)
    {
        Tcontainer partprod;

        partprod.reserve(arr.size() - window_size + 1);
        for (const auto& window : arr | std::views::slide(window_size))
            partprod.push_back(
                std::accumulate(window.begin(), window.end(), 1.0, std::multiplies<>()));

        arr.swap(partprod);
    }

    // One to One

    template <typename Tdata>
    inline void cumsum(std::span<Tdata> arr)
    {
        Tdata cumsum = { 0.0 };
        for (Tdata& val : arr)
            val = cumsum += val;
    }

    // TODO: Tdata must be signed
    template <typename Tdata>
    inline void cumaltsum(std::span<Tdata> arr)
    {
        Tdata cumaltsum = 0.0;
        Tdata sign = -1.0;
        for (Tdata& val : arr)
            val = cumaltsum += val * (sign = -sign);
    }

    template <typename Tdata>
    inline void cumprod(std::span<Tdata> arr)
    {
        double cumprod = 1.0;
        for (double& num : arr)
            num = cumprod *= num;
    }

    // TODO: factorial

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

    // TODO:

    // Many to One

    // TODO:
} // namespace xph::linalg

#endif /* ifndef HEADER_SCRIPTS_CXX_LINALG_ */
