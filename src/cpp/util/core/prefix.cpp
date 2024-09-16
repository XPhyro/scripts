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

#define OPT(NAME) lyra::opt(m_##NAME)

DEFINE_EXEC_INFO()

class Options {
    DECLARE_FIELD(bool, prefix_time, false);
    DECLARE_FIELD(bool, prefix_delta, false);
    DECLARE_FIELD(bool, prefix_line, false);

public:
    Options() = delete;

    Options(int argc, char** argv)
    {
        bool help = false;
        auto cli = lyra::cli() |
                   lyra::help(help).description("Prefix lines with selected attributes.") |
                   OPT(prefix_time)["-t"]["--time"]("prefix lines with current time") |
                   OPT(prefix_delta)["-d"]["--delta"]("prefix lines with delta time") |
                   OPT(prefix_line)["-l"]["--line"]("prefix lines with line number");
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
