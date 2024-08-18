#include "options.hpp"

#include <iostream>

#include <cstdlib>

#include <lyra/lyra.hpp>

#include <xph/exec_info.hpp>
#include <xph/lexical_cast.hpp>

#include "command.hpp"

paf::Options::Options(int argc, char** argv)
{
    bool help = false;
    auto cli = lyra::cli() |
               lyra::help(help).description("Pathfinding interface for interactive shells.");

    paf::Alias alias{ cli };
    paf::Mark mark{ cli };
    paf::Unmark unmark{ cli };
    paf::ToggleMark toggle_markj{ cli };
    paf::Jump jump{ cli };
    paf::Open open{ cli };
    paf::Print print{ cli };
    paf::List list{ cli };

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
