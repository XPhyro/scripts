#ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_
#define HEADER_SCRIPTS_CXX_AUTO_TIMER_

#include <memory>

#include "logger.hpp"
#include "tictoc.hpp"
#include "units.hpp"

namespace xph {
    class auto_timer {
    private:
        std::shared_ptr<logger> m_logger;
        const xph::log_level km_log_level = xph::log_level::k_verbose;

        const std::string km_identifier;
        bool dumped;

    public:
        inline auto_timer() = delete;

        inline auto_timer(std::shared_ptr<logger> logger, const std::string& identifier)
            : m_logger(logger)
            , km_identifier(identifier)
            , dumped(false)
        {
            xph::tic(km_identifier);
        }

        inline ~auto_timer()
        {
            manual_dump();
        }

        inline void manual_dump()
        {
            if (dumped)
                return;

            dumped = true;

            auto duration = xph::toc(km_identifier).count();
            if (!duration)
                return;

            m_logger->log(
                km_log_level, km_identifier, " took ", xph::unit::tick_to_human(duration));
        }
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_TIMER_ */
