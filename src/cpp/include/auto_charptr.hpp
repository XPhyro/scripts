#ifndef HEADER_SCRIPTS_CXX_AUTO_CHARPTR_
#define HEADER_SCRIPTS_CXX_AUTO_CHARPTR_

#include <cstdlib>

namespace xph {
    class auto_charptr {
    private:
        char* m_ptr;

    public:
        inline auto_charptr() = delete;

        inline auto_charptr(char* ptr) : m_ptr(ptr) {}

        inline ~auto_charptr()
        {
            std::free(m_ptr);
        }
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_CHARPTR_ */
