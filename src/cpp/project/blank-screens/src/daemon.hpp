#ifndef HEADER_SCRIPTS_CXX_BS_DAEMON_
#define HEADER_SCRIPTS_CXX_BS_DAEMON_

#include "blind.hpp"
#include "cli.hpp"
#include <X11/Xlib.h>

namespace bs {
    class daemon {
    private:
        const cli& m_cli;
        blinds m_blinds;

    public:
        daemon(void) = delete;
        daemon(const cli& cli);
        [[noreturn]] void run(void);
        [[noreturn]] void handle_signals(int signal);

    private:
        void dispatch(const std::string& command_line);
        void clean_up(bool lerp_to_zero = true);
    };
} // namespace bs

#endif /* ifndef HEADER_SCRIPTS_CXX_BS_DAEMON_ */
