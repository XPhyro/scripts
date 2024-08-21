#include "cli.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include <cstdlib>

#include <lyra/lyra.hpp>

#include <xph/exec_info.hpp>

bs::cli::cli(int argc, char** argv)
{
    bool show_help = false;
    double fps = 240;
    auto cli =
        lyra::cli() | lyra::help(show_help).description("Blank monitors.") |
        lyra::opt(m_alpha, "num")["-a"]["--alpha"]("set initial alpha of blinds") |
        lyra::opt(m_fifo_path, "path")["-c"]["--command-fifo"]("listen for commands on fifo") |
        lyra::opt(fps, "num")["-f"]["--fps"]("set frame rate of alpha interpolation") |
        lyra::opt(m_max_alpha, "num")["-H"]["--high"]("set upper alpha bound") |
        lyra::opt(m_min_alpha, "num")["-L"]["--low"]("set lower alpha bound") |
        lyra::opt(m_lock_path, "path")["-l"]["--lock"]("set lock path") |
        lyra::opt(m_ignore_primary)["-p"]["--ignore-primary"]("ignore primary monitor") |
        lyra::opt(m_snap_threshold, "num")["-s"]["--snap"]("set alpha snap threshold") |
        lyra::opt(m_lerp_factor, "num")["-t"]["--factor"]("set alpha linear interpolation factor") |
        lyra::arg(m_monitors, "monitor")(
            "initial monitor to blank, can be @cursor to use the monitor the cursor is in")
            .cardinality(0, std::numeric_limits<size_t>::max());

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

    m_frame_time = std::chrono::duration<double>{ 1.0 / fps };

    std::sort(m_monitors.begin(), m_monitors.end());
    m_monitors.erase(std::unique(m_monitors.begin(), m_monitors.end()), m_monitors.end());
}

double bs::cli::alpha(void) const noexcept
{
    return m_alpha;
}

const std::string& bs::cli::fifo_path(void) const noexcept
{
    return m_fifo_path;
}

const std::chrono::duration<double>& bs::cli::frame_time(void) const noexcept
{
    return m_frame_time;
}

double bs::cli::max_alpha(void) const noexcept
{
    return m_max_alpha;
}

double bs::cli::min_alpha(void) const noexcept
{
    return m_min_alpha;
}

const std::string& bs::cli::lock_path(void) const noexcept
{
    return m_lock_path;
}

bool bs::cli::ignore_primary(void) const noexcept
{
    return m_ignore_primary;
}

double bs::cli::snap_threshold(void) const noexcept
{
    return m_snap_threshold;
}

double bs::cli::lerp_factor(void) const noexcept
{
    return m_lerp_factor;
}

const std::vector<std::string>& bs::cli::monitors(void) const noexcept
{
    return m_monitors;
}

namespace bs {
    std::ostream& operator<<(std::ostream& os, const bs::cli& cli)
    {
        os << "alpha: " << cli.alpha() << '\n';
        os << "fifo path: " << cli.fifo_path() << '\n';
        os << "frame time: " << cli.frame_time().count() << '\n';
        os << "max alpha: " << cli.max_alpha() << '\n';
        os << "min alpha: " << cli.min_alpha() << '\n';
        os << "lock path: " << cli.lock_path() << '\n';
        os << "ignore primary: " << std::boolalpha << cli.ignore_primary() << '\n';
        os << "snap threshold: " << cli.snap_threshold() << '\n';
        os << "lerp factor: " << cli.lerp_factor() << '\n';

        return os;
    }
} // namespace bs
