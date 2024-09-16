#include <chrono>
#include <iostream>
#include <string>

#include <cstdlib>

#include <lyra/lyra.hpp>

#include <xph/exec_info.hpp>
#include <xph/units.hpp>

#define DECLARE_FIELD(TYPE, NAME, DEFAULT)                   \
public:                                                      \
    static const constexpr TYPE km_default_##NAME = DEFAULT; \
                                                             \
private:                                                     \
    TYPE m_##NAME = km_default_##NAME;                       \
                                                             \
public:                                                      \
    [[nodiscard]] TYPE NAME() const noexcept                 \
    {                                                        \
        return m_##NAME;                                     \
    }

#define OPT()                                                \
    X_OPT(bool, prefix_time, false, t, time, "current time") \
    X_OPT(bool, prefix_delta, false, d, delta, "delta time") \
    X_OPT(bool, prefix_line, false, l, line, "line number")

DEFINE_EXEC_INFO()

class Options {
#define X_OPT(X_TYPE, X_NAME, X_DEFAULT, X_SHORT, X_LONG, X_DESC) \
    DECLARE_FIELD(X_TYPE, X_NAME, X_DEFAULT);
    OPT()
#undef X_OPT

public:
    Options() = delete;

    Options(int argc, char** argv)
    {
        bool help = false;
#define X_OPT(X_TYPE, X_NAME, X_DEFAULT, X_SHORT, X_LONG, X_DESC) \
    | lyra::opt(m_##X_NAME)["-" #X_SHORT]["--" #X_LONG]("prefix lines with " X_DESC)
        auto cli = lyra::cli() |
                   lyra::help(help).description("Prefix lines with selected attributes.") OPT();
#undef X_OPT
        auto args = cli.parse({ argc, argv });

        if (!args) {
            std::cerr << args.message() << '\n';
            std::cerr << "Try '" << xph::exec_path << " -h' for more information.\n";
            std::exit(EXIT_FAILURE);
        }

        if (help) {
            std::cout << cli << '\n';
            std::exit(EXIT_SUCCESS);
        }
    }
};

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    Options options(argc, argv);

    std::chrono::steady_clock clock;

    auto start_time = clock.now();
    auto last_time = start_time;

    for (std::string line; std::getline(std::cin, line);) {
        auto current_time = clock.now();

        bool prefixed = false;
        if (options.prefix_line()) {
            static int line_number = 0;

            std::cout << "[#" << line_number++;

            prefixed = true;
        }

        if (options.prefix_time()) {
            auto elapsed_time = current_time - start_time;
            auto elapsed_ticks = elapsed_time.count();

            if (prefixed)
                std::cout << " | ";
            else
                std::cout << '[';

            std::cout << '@';
            xph::unit::write_tick_as_human(std::cout, elapsed_ticks);

            prefixed = true;
        }

        if (options.prefix_delta()) {
            auto differential_time = current_time - last_time;
            auto differential_ticks = differential_time.count();

            if (prefixed)
                std::cout << " | ";
            else
                std::cout << '[';

            std::cout << '+';
            xph::unit::write_tick_as_human(std::cout, differential_ticks);

            prefixed = true;
        }

        if (prefixed)
            std::cout << "]: ";

        std::cout << line << '\n';

        last_time = current_time;
    }

    return EXIT_SUCCESS;
}
