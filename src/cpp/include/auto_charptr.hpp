#ifndef HEADER_SCRIPTS_CXX_AUTO_CHARPTR_
#define HEADER_SCRIPTS_CXX_AUTO_CHARPTR_

#include <cstdlib>

namespace xph {
    class auto_charptr {
    private:
        char* m_ptr;

    public:
        inline auto_charptr() = delete;

        inline auto_charptr(char* ptr) noexcept : m_ptr(ptr) {}

        inline ~auto_charptr() noexcept
        {
            std::free(m_ptr);
        }

        inline const char* value() const
        {
            return m_ptr;
        }

        inline char* value()
        {
            return m_ptr;
        }

        inline void overwrite(char* new_ptr) noexcept
        {
            std::free(m_ptr);
            m_ptr = new_ptr;
        }
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_AUTO_CHARPTR_ */
