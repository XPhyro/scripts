#include "options.hpp"

#include <cstdlib>
#include <iostream>

#include <unistd.h>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/lexical_cast.hpp>

igfl::Options::Options(int& argc, char**& argv)
{
    for (int i; (i = getopt(argc, argv, "d:F:f:hI:L:m")) != -1;) {
        switch (i) {
            case 'd': {
                init_dir = { optarg };
                xph::die_if(!init_dir->size(), "initial directory cannot be empty (-d)");
            } break;
            case 'F': {
                idle_fps = xph::lexical_cast<char*, decltype(idle_fps)>(optarg);
                xph::die_if(idle_fps <= 0, "idle FPS must be positive (-F)");
            } break;
            case 'f': {
                active_fps = xph::lexical_cast<char*, decltype(active_fps)>(optarg);
                xph::die_if(active_fps <= 0, "active FPS must be positive (-f)");
            } break;
            case 'h': {
                // TODO: show defaults
                std::cout << "Usage: " << xph::exec_path << " [OPTION]... [FILE]...\n"
                          << "A file explorer that uses ImGui, ImTui and ncurses, written in C++.\n"
                             "\n"
                             "  -d DIR   change directory to DIR on startup\n"
                             "  -F FPS   set idle FPS\n"
                             "  -f FPS   set active FPS\n"
                             "  -h       display this help and exit\n"
                             "  -I FILE  set ini file path for ImGui\n"
                             "  -L FILE  set log file path for ImGui\n"
                             "  -m       disable mouse support\n";
                std::exit(EXIT_SUCCESS);
            }
            case 'I': {
                ini_file = { optarg };
                xph::die_if(!ini_file->size(), "ini file cannot be empty (-I)");
            } break;
            case 'L': {
                log_file = { optarg };
                xph::die_if(!log_file->size(), "log file cannot be empty (-L)");
            } break;
            case 'm': {
                mouse_support = true;
            } break;
            default: {
                std::cerr << "Try '" << xph::exec_path << " -h' for more information.\n";
                std::exit(EXIT_FAILURE);
            }
        }
    }

    argc -= optind;
    argv += optind;
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
IMPLEMENT_GET(mouse_support)

#undef IMPLEMENT_GET
