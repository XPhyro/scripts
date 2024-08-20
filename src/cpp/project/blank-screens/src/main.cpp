#include <filesystem>

#include <cstdlib>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/sys.hpp>

#include "cli.hpp"
#include "daemon.hpp"

DEFINE_EXEC_INFO();

namespace fs = std::filesystem;

const bs::daemon* g_daemon = nullptr;

void handle_signal(int signal)
{
    if (g_daemon)
        const_cast<bs::daemon*>(g_daemon)->handle_signals(signal);
}

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);
    bs::cli cli(argc, argv);

    std::cerr << cli;

    std::error_code ec;
    if (fs::create_directory(cli.lock_path(), ec)) {
        bs::daemon daemon{ cli };

        g_daemon = &daemon;
        xph::sys::signals<4>({ SIGINT, SIGTERM, SIGQUIT, SIGHUP }, handle_signal);

        daemon.run();
        return EXIT_SUCCESS;
    }

    xph::die_if(ec == std::errc::file_exists, "lock is held by another daemon");
    xph::die("could not acquire daemon lock");

    return EXIT_SUCCESS;
}
