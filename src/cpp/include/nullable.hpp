#ifndef HEADER_SCRIPTS_CXX_NULLABLE
#define HEADER_SCRIPTS_CXX_NULLABLE

#include <ostream>
#include <utility>

namespace xph
{
template <typename T>
class nullable {
public:
    nullable() {}

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
        is_null = false;
        value = t;
        return *this;
    }

    bool operator==(const T& t) const
    {
        return value == t;
    }

    bool operator!=(const T& t) const
    {
        return value != t;
    }

    friend std::ostream& operator<<(std::ostream& os, const nullable& n)
    {
        os << n.value;
        return os;
    }

    operator const T&() const
    {
        return std::move(value);
    }

    operator bool() const
    {
        return !is_null;
    }

private:
    bool is_null = true;
    T value;
};
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_NULLABLE */
