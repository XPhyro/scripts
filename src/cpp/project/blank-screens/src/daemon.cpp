#include "daemon.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>

#include <csignal>
#include <cstdlib>

#include <sys/stat.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include <xph/die.hpp>
#include <xph/math.hpp>

namespace fs = std::filesystem;

// // PUBLIC // //

bs::daemon::daemon(const cli& cli) : m_cli(cli), m_blinds({ cli }) {}

[[noreturn]] void bs::daemon::run()
{
    if (::mkfifo(m_cli.fifo_path().c_str(), 0600))
        xph::die("could not create fifo");

    for (;;) {
        std::ifstream fifo{ m_cli.fifo_path() };
        if (!fifo.is_open())
            xph::die("could not open fifo");

        for (std::string line; std::getline(fifo, line); this->dispatch(line)) {}
    }

    std::exit(EXIT_FAILURE);
}

void bs::daemon::handle_signals(int signal)
{
    auto interrupt = signal == SIGINT;
    clean_up(interrupt);
    std::exit(!interrupt);
}

// // PRIVATE // //

void bs::daemon::dispatch(const std::string& command_line)
{
    static std::vector<std::string> argv{};
    static std::istringstream iss;
    static std::string token;

    argv.clear();
    iss.clear();
    iss.str(command_line);

    for (iss.str(command_line); iss >> token; argv.push_back(token)) {}

    if (argv.empty())
        return;

    if (argv[0] == "exit") {
        clean_up();
        std::exit(EXIT_SUCCESS);
    }

    if (argv[0] == "alpha") {
        std::cerr << xph::exec_name << ": setting alpha to " << argv[1] << '\n';
        const auto alpha = argv.size() < 2 ? m_cli.alpha() : std::stod(argv[1]);
        const auto alpha_is_relative = alpha < 0.0 || argv[1].front() == '+';
        if (argv.size() < 3) {
            m_blinds.lerp_alpha(alpha, std::nullopt, alpha_is_relative);
        } else {
            for (const auto& monitor : argv | std::views::drop(2))
                m_blinds.lerp_alpha(alpha, monitor, alpha_is_relative);
        }
    } else if (argv[0] == "add") {
        for (const auto& monitor : argv | std::views::drop(1))
            m_blinds.add_monitor(monitor, false);
        m_blinds.commit_monitor_changes();
    } else if (argv[0] == "remove") {
        for (const auto& monitor : argv | std::views::drop(1))
            m_blinds.remove_monitor(monitor, false);
        m_blinds.commit_monitor_changes();
    } else if (argv[0] == "toggle") {
        for (const auto& monitor : argv | std::views::drop(1))
            m_blinds.toggle_monitor(monitor, false);
        m_blinds.commit_monitor_changes();
    } else if (argv[0] == "clear") {
        m_blinds.clear_monitors(true);
    } else {
        std::cerr << xph::exec_name << ": unknown command [" << argv[0] << "]\n";
    }
}

void bs::daemon::clean_up(bool lerp_to_zero)
{
    fs::remove(m_cli.fifo_path());
    fs::remove_all(m_cli.lock_path());

    if (lerp_to_zero)
        m_blinds.lerp_alpha(0.0);
}
