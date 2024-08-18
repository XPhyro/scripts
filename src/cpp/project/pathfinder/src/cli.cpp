#include "cli.hpp"

#include <iostream>

#include <cstdlib>

#include <lyra/lyra.hpp>

#include <xph/exec_info.hpp>

#include "cmd.hpp"

paf::cli::cli(int argc, char** argv)
{
    bool help = false;
    auto cli = lyra::cli() |
               lyra::help(help).description("Pathfinding interface for interactive shells.");

    paf::alias _{ cli };
    paf::mark _{ cli };
    paf::unmark _{ cli };
    paf::jump _{ cli };
    paf::open _{ cli };
    paf::print _{ cli };
    paf::list _{ cli };

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

[[noreturn]] void paf::cli::run(void)
{
    std::exit(EXIT_SUCCESS);
}
