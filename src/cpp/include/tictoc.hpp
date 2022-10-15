#ifndef HEADER_SCRIPTS_CXX_SYSUTIL_
#define HEADER_SCRIPTS_CXX_SYSUTIL_

#include <chrono>
#include <string>
#include <unordered_map>

namespace xph {
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> times{};

    void tic(const std::string& hash);

    inline void tic(const std::string& hash)
    {
        times[hash] = std::chrono::steady_clock::now();
    }

    [[nodiscard]] auto toc(const std::string& hash);

    [[nodiscard]] inline auto toc(const std::string& hash)
    {
        return std::chrono::steady_clock::now() - times[hash];
    }

    auto toctic(const std::string& hash);

    inline auto toctic(const std::string& hash)
    {
        auto time_point = std::chrono::steady_clock::now();
        auto ret = time_point - times[hash];
        times[hash] = time_point;
        return ret;
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_SYSUTIL_ */
