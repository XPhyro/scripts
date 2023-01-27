#ifndef HEADER_SCRIPTS_CXX_TUPLE_HASH_
#define HEADER_SCRIPTS_CXX_TUPLE_HASH_

#include <functional>
#include <tuple>

// taken from <https://stackoverflow.com/a/7115547/8403070>

namespace std {
    namespace {

        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, const T& v)
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl {
            static void apply(size_t& seed, const Tuple& tuple)
            {
                HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
                hash_combine(seed, std::get<Index>(tuple));
            }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple, 0> {
            static void apply(size_t& seed, const Tuple& tuple)
            {
                hash_combine(seed, std::get<0>(tuple));
            }
        };
    } // namespace

    template <typename... TT>
    struct hash<std::tuple<TT...>> {
        size_t operator()(const std::tuple<TT...>& tt) const
        {
            size_t seed = 0;
            HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
            return seed;
        }
    };
} // namespace std

#endif /* ifndef HEADER_SCRIPTS_CXX_TUPLE_HASH_ */
