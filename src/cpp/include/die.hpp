#ifndef HEADER_SCRIPTS_CXX_DIE_
#define HEADER_SCRIPTS_CXX_DIE_

#include <iostream>

#include <exec_info.hpp>

namespace xph {
    template <typename... Ts>
    [[noreturn]] void die(const Ts&... args);

    template <typename... Ts>
    [[noreturn]] inline void die(const Ts&... args)
    {
        std::cerr << xph::exec_name << ": ";
        (
            [&] {
                std::cerr << args;
            }(),
            ...);
        std::cerr << '\n';
        std::exit(EXIT_FAILURE);
    }

    template <typename... Ts>
    void die_if(bool condition, const Ts&... args);

    template <typename... Ts>
    inline void die_if(bool condition, const Ts&... args)
    {
        if (condition)
            die(args...);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_DIE_ */
