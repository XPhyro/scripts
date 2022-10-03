#ifndef HEADER_SCRIPTS_CXX_DIE
#define HEADER_SCRIPTS_CXX_DIE

#include <iostream>

extern const char* execname;

namespace xph {
    template <typename... Ts>
    [[noreturn]] void die(const Ts&... args);

    template <typename... Ts>
    [[noreturn]] inline void die(const Ts&... args)
    {
        std::cerr << execname << ": ";
        (
            [&] {
                std::cerr << args;
            }(),
            ...);
        std::cerr << '\n';
        std::exit(EXIT_FAILURE);
    }
} // namespace xph

#define CAPTURE_EXECNAME()  \
    if (argc)               \
        execname = argv[0]; \
    else                    \
        execname = "NULL"

#endif /* ifndef HEADER_SCRIPTS_CXX_DIE */
