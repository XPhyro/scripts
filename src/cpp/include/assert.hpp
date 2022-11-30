#ifndef HEADER_SCRIPTS_CXX_ASSERT_
#define HEADER_SCRIPTS_CXX_ASSERT_

#include <iostream>

#include <exec_info.hpp>

namespace xph {
    template <typename... Ts>
    void assert(bool condition, const Ts&... args);

    template <typename... Ts>
    inline void assert(bool condition, const Ts&... args)
    {
        if (condition)
            return;

        std::cerr << xph::exec_name << ": ";
        (
            [&] {
                std::cerr << args;
            }(),
            ...);
        std::cerr << '\n';
        std::exit(EXIT_FAILURE);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_ASSERT_ */
