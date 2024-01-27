#include "options.hpp"

#include <cstdlib>
#include <iostream>

#include <unistd.h>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/lexical_cast.hpp>

#include "lib/lyra/lyra.hpp"

igfl::Options::Options(int argc, char** argv)
{
    bool help = false;
    auto cli =
        lyra::cli() | lyra::help(help) |
        lyra::opt(init_dir, "dir_path")["-d"]["--init-dir"]("change to directory on startup") |
        lyra::opt(idle_fps, "fps")["--idle-fps"]("set idle FPS") |
        lyra::opt(active_fps, "fps")["--active-fps"]("set active FPS") |
        lyra::opt(ini_file, "file_path")["--ini-file"]("set ini file path for ImGui") |
        lyra::opt(log_file, "file_path")["--log-file"]("set log file path for ImGui") |
        lyra::opt(disable_mouse_support)["-m"]["--no-mouse"]("disable mouse support");
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

    xph::die_if(init_dir && !init_dir->size(), "initial directory cannot be empty (-d)");
    xph::die_if(idle_fps <= 0, "idle FPS must be positive (-F)");
    xph::die_if(active_fps <= 0, "active FPS must be positive (-f)");
    xph::die_if(ini_file && !ini_file->size(), "ini file cannot be empty (-I)");
    xph::die_if(log_file && !log_file->size(), "log file cannot be empty (-L)");
}

#define IMPLEMENT_GET(FIELD)                                                                 \
    [[nodiscard]] decltype(igfl::Options::FIELD) igfl::Options::get_##FIELD() const noexcept \
    {                                                                                        \
        return FIELD;                                                                        \
    }

IMPLEMENT_GET(init_dir)
IMPLEMENT_GET(idle_fps)
IMPLEMENT_GET(active_fps)
IMPLEMENT_GET(ini_file)
IMPLEMENT_GET(log_file)
IMPLEMENT_GET(disable_mouse_support)

#undef IMPLEMENT_GET
