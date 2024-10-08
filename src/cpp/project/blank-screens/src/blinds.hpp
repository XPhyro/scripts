#ifndef HEADER_SCRIPTS_CXX_BS_BLIND_
#define HEADER_SCRIPTS_CXX_BS_BLIND_

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <X11/extensions/Xrandr.h>

#include "cli.hpp"

namespace bs {
    class blinds {
    public:
        static const constexpr std::string_view k_expr_special_prefix = "@";
        static const constexpr std::string_view k_expr_cursor = "cursor";
        static const constexpr std::string_view k_expr_invert_prefix = "~";

    private:
        const cli& m_cli;
        Display* const m_display;
        const int m_screen;
        const Window m_root_window;
        std::vector<std::string> m_monitors;
        std::vector<Window> m_windows;
        std::vector<double> m_alphas;

    public:
        blinds(void) = delete;
        ~blinds(void);
        blinds(const cli& cli);

        bool add_monitor(const std::string& monitor_expr, bool commit_changes = true);
        bool remove_monitor(const std::string& monitor_expr, bool commit_changes = true);
        void toggle_monitor(const std::string& monitor_expr, bool commit_changes = true);
        void clear_monitors(bool commit_changes = true);
        void commit_monitor_changes(void);
        void lerp_alpha(double alpha,
                        std::optional<std::string> monitor = std::nullopt,
                        bool alpha_is_relative = false);

    private:
        std::optional<std::string> eval_monitor_expr(const std::string& monitor_expr);
        void update_windows(void);
        void create_windows(void);
        void destroy_windows(void);
        double set_window_alpha(Window window, double alpha);
    };
} // namespace bs

#endif /* ifndef HEADER_SCRIPTS_CXX_BS_BLIND_ */
