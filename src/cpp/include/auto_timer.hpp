#ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_
#define HEADER_SCRIPTS_CXX_AUTO_TIMER_

#include <log.hpp>

namespace xph {
    class AutoTimer {
    private:
        std::shared_ptr<Logger> m_logger;
        const xph::log_level km_log_level = xph::log_level::k_verbose;

        const std::string km_identifier;
        bool dumped;

    public:
        inline AutoTimer() = delete;

        inline AutoTimer(std::shared_ptr<Logger> logger, const std::string& identifier)
            : m_logger(logger)
            , km_identifier(identifier)
            , dumped(false)
        {
            xph::tic(km_identifier);
        }

        inline ~AutoTimer()
        {
            manual_dump();
        }

        inline void manual_dump()
        {
            if (dumped)
                return;

            dumped = true;

            long double scaled_duration;
            const char* unit;

            if (auto duration = xph::toc(km_identifier).count(); !duration) {
                return;
            } else {
                if (duration > 31'556'952'000'000'000) {
                    scaled_duration = duration / 31'556'952'000'000'000.0l;
                    unit = "y";
                } else if (duration > 2'629'746'000'000'000) {
                    scaled_duration = duration / 2'629'746'000'000'000.0l;
                    unit = "m";
                } else if (duration > 604'800'000'000'000) {
                    scaled_duration = duration / 604'800'000'000'000.0l;
                    unit = "w";
                } else if (duration > 86'400'000'000'000) {
                    scaled_duration = duration / 86'400'000'000'000.0l;
                    unit = "d";
                } else if (duration > 3'600'000'000'000) {
                    scaled_duration = duration / 3'600'000'000'000.0l;
                    unit = "h";
                } else if (duration > 60'000'000'000) {
                    scaled_duration = duration / 60'000'000'000.0l;
                    unit = "m";
                } else if (duration > 1'000'000'000) {
                    scaled_duration = duration / 1'000'000'000.0l;
                    unit = "s";
                } else if (duration > 1'000'000) {
                    scaled_duration = duration / 1'000'000.0l;
                    unit = "ms";
                } else if (duration > 1'000) {
                    scaled_duration = duration / 1'000.0l;
                    unit = "us";
                } else {
                    scaled_duration = duration;
                    unit = "ns";
                }
            }

            m_logger->log(km_log_level, km_identifier, " took ", scaled_duration, unit);
        }
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_ */
