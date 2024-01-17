#ifndef HEADER_SCRIPTS_CXX_POOL_
#define HEADER_SCRIPTS_CXX_POOL_

#include <optional>
#include <span>
#include <vector>

namespace xph {
    template <typename T>
    class pool {
    private:
        std::size_t m_max_pool_size;
        std::vector<T> m_pool;

    public:
        inline pool(std::size_t max_pool_size) : m_max_pool_size(max_pool_size) {}

        inline std::size_t max_size()
        {
            return m_max_pool_size;
        }

        inline std::size_t size()
        {
            return m_pool.size();
        }

        inline std::size_t capacity()
        {
            return m_pool.capacity();
        }

        inline T& at(std::size_t index) const
        {
            return m_pool[index];
        }

        inline std::span<T> view() const
        {
            return { m_pool };
        }

        inline std::span<T> view(std::size_t begin, std::size_t end) const
        {
            return { m_pool.begin() + begin, m_pool.begin() + end };
        }

        inline void push(T val)
        {
            if (!m_max_pool_size)
                return;

            if (m_pool.size() == m_max_pool_size)
                m_pool.erase(m_pool.begin());

            m_pool.push_back(val);
        }

        inline std::optional<T> pop()
        {
            if (!m_pool.size())
                return std::nullopt;

            auto front = m_pool.front();
            m_pool.erase(m_pool.begin());
            return { front };
        }
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_POOL_ */
