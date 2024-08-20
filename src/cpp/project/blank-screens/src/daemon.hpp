#ifndef HEADER_SCRIPTS_CXX_BS_DAEMON_
#define HEADER_SCRIPTS_CXX_BS_DAEMON_

#include "blind.hpp"
#include "cli.hpp"
#include <X11/Xlib.h>

namespace bs {
    class daemon {
    private:
        const constexpr static double km_epsilon = 0.0001;
        const cli& m_cli;
        std::vector<blind> m_blinds;
        Display* m_display;
        double m_last_alpha;

    public:
        daemon(void) = delete;
        daemon(const cli& cli);
        [[noreturn]] void run(void);
        void handle_signals(int signal);

    private:
        void dispatch(const std::string& command_line);
        void lerp_alpha(double alpha);
        void add_monitor(const std::string& target_monitor);
        void remove_monitor(const std::string& target_monitor);
        void toggle_monitor(const std::string& target_monitor);
    };
} // namespace bs

#endif /* ifndef HEADER_SCRIPTS_CXX_BS_DAEMON_ */
