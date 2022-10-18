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

    inline std::string lexical_cast(const bool& b)
    {
        std::ostringstream ss;
        ss << std::boolalpha << b;
        return ss.str();
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_LEXICAL_CAST_ */
