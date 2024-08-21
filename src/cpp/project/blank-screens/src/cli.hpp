#ifndef HEADER_SCRIPTS_CXX_PAF_CLI_
#define HEADER_SCRIPTS_CXX_PAF_CLI_

#include <chrono>
#include <string>
#include <vector>

namespace bs {
    class cli {
    public:
        static const constexpr double k_default_alpha = 1.0;
        static const constexpr double k_default_frame_rate = 100.0;
        static const constexpr double k_default_max_alpha = 1.0;
        static const constexpr double k_default_min_alpha = 0.0;
        static const constexpr double k_default_snap_threshold = 0.01;
        static const constexpr double k_default_lerp_factor = 0.0625;
        static const constexpr char* const k_default_fifo_path = "/tmp/blank-screens/fifo";
        static const constexpr char* const k_default_lock_path = "/tmp/blank-screens/";

    private:
        double m_alpha = k_default_alpha;
        std::string m_fifo_path = k_default_fifo_path;
        std::chrono::duration<double> m_frame_time{ 1.0 / k_default_frame_rate };
        double m_max_alpha = k_default_max_alpha;
        double m_min_alpha = k_default_min_alpha;
        std::string m_lock_path = k_default_lock_path;
        bool m_ignore_primary = false;
        double m_snap_threshold = k_default_snap_threshold;
        double m_lerp_factor = k_default_lerp_factor;
        std::vector<std::string> m_monitors;

    public:
        cli(void) = delete;
        cli(int argc, char** argv);

        double alpha(void) const noexcept;
        const std::string& fifo_path(void) const noexcept;
        const std::chrono::duration<double>& frame_time(void) const noexcept;
        double max_alpha(void) const noexcept;
        double min_alpha(void) const noexcept;
        const std::string& lock_path(void) const noexcept;
        bool ignore_primary(void) const noexcept;
        double snap_threshold(void) const noexcept;
        double lerp_factor(void) const noexcept;
        const std::vector<std::string>& monitors(void) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const cli& cli);
    };
} // namespace bs

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_CLI_ */
