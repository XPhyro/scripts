#ifndef HEADER_SCRIPTS_CXX_VIEWS_
#define HEADER_SCRIPTS_CXX_VIEWS_

#include <cstddef>
#include <iterator>
#include <tuple>
#include <utility>

namespace xph {
    template <std::ranges::range Titerable, std::ranges::range Titerator>
    constexpr inline Titerable&& enumerate(Titerable&& iterable);

    template <std::ranges::range Titerable, std::ranges::range Titerator>
    constexpr Titerable&& enumerate(Titerable&& iterable)
    {
        class iterator_t {
            std::size_t iteration = 0;
            Titerator iterator;

        public:
            iterator_t(Titerator&& iterator) : iterator(iterator) {}

            bool operator!=(const iterator_t& other) const
            {
                return iterator != other.iterator;
            }

            void operator++()
            {
                ++iteration;
                ++iterator;
            }

            auto operator*() const
            {
                return std::move(std::tie(iteration, *iterator));
            }
        };

        class iterable_t {
            Titerable iterable;

        public:
            iterator_t&& begin() const
            {
                return std::forward(iterator_t(std::begin(iterable)));
            }

            iterator_t&& end() const
            {
                return std::forward(iterator_t(std::end(iterable)));
            }
        };

        return std::forward(iterable_t(std::forward(iterable)));
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_VIEWS_ */
