#include <cerrno>
#include <cstdlib>

#include <unistd.h>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/param.hpp>

#include "cli.hpp"

DEFINE_EXEC_INFO();

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);
    paf::cli cli(argc, argv);
    cli.run();
}