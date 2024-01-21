#ifndef HEADER_SCRIPTS_CXX_LOGGER_
#define HEADER_SCRIPTS_CXX_LOGGER_

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include <unistd.h>

#include "exec_info.hpp"
#include "macros.hpp"
#include "tictoc.hpp"

namespace xph {
    using log_level_underlying_t = int;

#define LOG_LEVELS           \
    X_LOG_LEVEL(min, -9)     \
    X_LOG_LEVEL(quiet, -8)   \
    X_LOG_LEVEL(panic, 0)    \
    X_LOG_LEVEL(fatal, 8)    \
    X_LOG_LEVEL(error, 16)   \
    X_LOG_LEVEL(warning, 24) \
    X_LOG_LEVEL(info, 32)    \
    X_LOG_LEVEL(verbose, 40) \
    X_LOG_LEVEL(debug, 48)   \
    X_LOG_LEVEL(trace, 56)   \
    X_LOG_LEVEL(max, 57)

#define X_LOG_LEVEL(name, value) value,
    const constexpr log_level_underlying_t log_level_values[] = { LOG_LEVELS };
#undef X_LOG_LEVEL

#define X_LOG_LEVEL(name, value) #name,
    const std::string log_level_names[] = { LOG_LEVELS };
#undef X_LOG_LEVEL

#define X_LOG_LEVEL(name, value) k_##name = value,
    enum class log_level { LOG_LEVELS };
#undef X_LOG_LEVEL

    const constexpr std::size_t log_level_count =
        sizeof(log_level_values) / sizeof(log_level_underlying_t);

    const std::string_view get_log_level_name(log_level level);

    class logger {
    private:
        xph::log_level m_log_level;

    public:
        static const constexpr xph::log_level kg_default_log_level =
#if NDEBUG
            xph::log_level::k_info
#else
            xph::log_level::k_debug
#endif
            ;

        static const constexpr char* const kg_default_log_level_string =
#if NDEBUG
            "info"
#else
            "debug"
#endif
            ;

        inline logger() : m_log_level(kg_default_log_level)
        {
            NOOP();
        }

        inline logger(xph::log_level log_level) : m_log_level(log_level)
        {
            NOOP();
        }

        template <typename... Ts>
        inline void log(xph::log_level log_level, const Ts&... args)
        {
            if (log_level > m_log_level)
                return;

            std::string level = log_level_names[log_level_count - 1];
            for (std::size_t i = 0; i < log_level_count; ++i) {
                if (static_cast<log_level_underlying_t>(log_level) <= log_level_values[i]) {
                    level = log_level_names[i];
                    break;
                }
            }

            std::cerr << xph::exec_name << '[' << level << ']' << ": ";
            (
                [&] {
                    std::cerr << args;
                }(),
                ...);
            std::cerr << '\n';
        }

        inline xph::log_level get_log_level()
        {
            return m_log_level;
        }

        inline void set_log_level(xph::log_level log_level)
        {
            m_log_level = log_level;
        }

        inline void output(void* buf, std::size_t buf_size)
        {
            write(STDOUT_FILENO, buf, buf_size);
        }
    };

} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_LOGGER_ */
