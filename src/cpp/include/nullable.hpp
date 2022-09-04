#ifndef HEADER_SCRIPTS_CXX_NULLABLE
#define HEADER_SCRIPTS_CXX_NULLABLE

#include <ostream>

namespace xph
{
template <typename T>
class nullable {
public:
    nullable(T t)
    {
        value = t;
    }

    nullable(T& t)
        : value(t)
    {
    }

    nullable(T&& t)
        : value(t)
    {
    }

    nullable operator=(const T& t)
    {
        is_null = t == T{};
        value = t;
        return *this;
    }

    bool operator==(const T& t)
    {
        return value == t;
    }

    bool operator!=(const T& t)
    {
        return value != t;
    }

    friend std::ostream& operator<<(std::ostream& os, const nullable& n)
    {
        os << n.value;
        return os;
    }

    operator const T&()
    {
        return std::move(value);
    }

    operator bool()
    {
        return !is_null;
    }

private:
    bool is_null = true;
    T value;
};
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_NULLABLE */
