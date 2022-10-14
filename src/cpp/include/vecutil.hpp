#ifndef HEADER_SCRIPTS_CXX_VECUTIL_
#define HEADER_SCRIPTS_CXX_VECUTIL_

#include <cstddef>
#include <utility>
#include <vector>

namespace xph::vec {
    template <typename T>
    std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T& val);

    template <typename T>
    inline std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T& val)
    {
        vec[idx] = val;
        return std::move(vec);
    }

    template <typename T>
    std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T&& val);

    template <typename T>
    inline std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T&& val)
    {
        vec[idx] = val;
        return std::move(vec);
    }

    template <typename T>
    std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T& val);

    template <typename T>
    inline std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T& val)
    {
        vec[idx] = val;
        return std::move(vec);
    }

    template <typename T>
    std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T&& val);

    template <typename T>
    inline std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T&& val)
    {
        vec[idx] = val;
        return std::move(vec);
    }
} // namespace xph::vec

#endif /* ifndef HEADER_SCRIPTS_CXX_VECUTIL_ */
