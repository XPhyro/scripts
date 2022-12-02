#ifndef HEADER_SCRIPTS_CXX_LEXICAL_CAST_
#define HEADER_SCRIPTS_CXX_LEXICAL_CAST_

#include <ios>
#include <sstream>
#include <string>

namespace xph {
    template <typename Targ, typename Tret>
    inline Tret lexical_cast(const Targ& arg)
    {
        std::istringstream ss(arg);
        Tret ret;
        ss >> ret;
        return ret;
    }

    template <typename Targ>
    inline bool lexical_cast(const Targ& arg)
    {
        std::istringstream ss(arg);
        bool b;
        ss >> std::boolalpha >> b;
        return b;
    }

    inline std::string lexical_cast(const bool& arg)
    {
        std::ostringstream ss;
        ss << std::boolalpha << arg;
        return ss.str();
    }

    inline std::string lexical_cast(const int& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const long& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const long long& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const unsigned& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const unsigned long& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const unsigned long long& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const float& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const double& arg)
    {
        return std::to_string(arg);
    }

    inline std::string lexical_cast(const long double& arg)
    {
        return std::to_string(arg);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_LEXICAL_CAST_ */
