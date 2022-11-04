#ifndef HEADER_SCRIPTS_CXX_EXEC_INFO_
#define HEADER_SCRIPTS_CXX_EXEC_INFO_

#include <string.h>

#define DEFINE_EXEC_INFO()     \
    namespace xph {            \
        const char* exec_name; \
        const char* exec_path; \
    }

namespace xph {
    extern const char* exec_name;
    extern const char* exec_path;

    inline void gather_exec_info(int argc, char* argv[])
    {
        if (argc) {
            exec_path = argv[0];
            if (auto str = strrchr(argv[0], '/'))
                exec_name = str + 1;
            else
                exec_name = exec_path;
        } else {
            exec_name = "NULL";
        }
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_EXEC_INFO_ */
