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
        underlying = t;
    }

    nullable(T& t)
        : underlying(t)
    {
    }

    nullable(T&& t)
        : underlying(t)
    {
    }

    nullable operator=(const T& other)
    {
        is_null = false;
        underlying = other;
        return *this;
    }

    operator bool() const
    {
        return !is_null;
    }

    operator const T&() const
    {
        return std::move(underlying);
    }

    bool is_set()
    {
        return !is_null;
    }

    bool value()
    {
        return !underlying;
    }

private:
    bool is_null = true;
    T underlying;
};
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_NULLABLE */
