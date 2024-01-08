#ifndef HEADER_SCRIPTS_CXX_ITERATORUTIL_
#define HEADER_SCRIPTS_CXX_ITERATORUTIL_

#include <iterator>

namespace xph {
    template <class Tcontainer>
    inline constexpr auto inserter(Tcontainer& container);

    template <class Tcontainer>
    inline constexpr auto inserter(Tcontainer& container)
    {
        return std::inserter(container, std::begin(container));
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_ITERATORUTIL_ */
