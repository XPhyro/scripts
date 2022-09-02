#ifndef HEADER_SCRIPTS_CXX_VECUTIL
#define HEADER_SCRIPTS_CXX_VECUTIL

#include <cstddef>
#include <vector>

namespace vecutil
{
template <typename T>
std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T& val)
{
    vec[idx] = val;
    return std::move(vec);
}

template <typename T>
std::vector<T> setindex(std::vector<T>&& vec, const std::size_t idx, const T&& val)
{
    vec[idx] = val;
    return std::move(vec);
}

template <typename T>
std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T& val)
{
    vec[idx] = val;
    return std::move(vec);
}

template <typename T>
std::vector<T> setindex(std::vector<T>&& vec, const std::size_t&& idx, const T&& val)
{
    vec[idx] = val;
    return std::move(vec);
}
} // namespace vecutil

#endif /* ifndef HEADER_SCRIPTS_CXX_VECUTIL */
