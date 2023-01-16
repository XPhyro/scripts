#ifndef HEADER_SCRIPTS_CXX_EXEC_INFO_
#define HEADER_SCRIPTS_CXX_EXEC_INFO_

#include <string>
#include <string_view>

#define DEFINE_EXEC_INFO()          \
    namespace xph {                 \
        std::string_view exec_name; \
        std::string_view exec_path; \
    }

namespace xph {
    extern std::string_view exec_name;
    extern std::string_view exec_path;

    inline void gather_exec_info(int argc, const char* argv[])
    {
        if (argc) {
            exec_path = argv[0];
            if (auto pos = exec_path.rfind('/'); pos != std::string::npos)
                exec_name = argv[0] + pos + 1;
            else
                exec_name = exec_path;
        } else {
            exec_name = "NULL";
        }
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_EXEC_INFO_ */
