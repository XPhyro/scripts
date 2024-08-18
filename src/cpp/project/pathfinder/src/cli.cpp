#include "cli.hpp"

#include <iostream>

#include <cstdlib>

#include <lyra/lyra.hpp>

#include <xph/exec_info.hpp>

#include "cmd.hpp"
#include "db.hpp"

paf::cli::cli(int argc, char** argv)
{
    bool show_help = false;
    bool reset_dir_db = false;
    bool reset_file_db = false;
    auto cli = lyra::cli() |
               lyra::help(show_help).description("Pathfinding interface for interactive shells.") |
               lyra::opt(reset_dir_db)["--reset-directory-database"]("reset directory database") |
               lyra::opt(reset_file_db)["--reset-file-database"]("reset file database");

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

    if (show_help) {
        std::cout << cli << '\n';
        std::exit(EXIT_SUCCESS);
    }

    if (reset_dir_db)
        paf::db::reset_db(paf::db_type::directory);

    if (reset_file_db)
        paf::db::reset_db(paf::db_type::file);
}

[[noreturn]] void paf::cli::run(void)
{
    std::exit(EXIT_SUCCESS);
}
