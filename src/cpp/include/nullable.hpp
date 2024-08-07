#ifndef HEADER_SCRIPTS_CXX_NULLABLE_
#define HEADER_SCRIPTS_CXX_NULLABLE_

#include <ostream>
#include <utility>

namespace xph {
    // std::optional is recommended instead of xph::nullable since C++17
    template <typename T>
    class nullable {
    public:
        nullable() : value{} {}

        nullable(const T& t) : value(t) {}

        nullable(T&& t) : value(t) {}

        nullable operator=(const T& other)
        {
            is_null = false;
            value = other;
            return *this;
        }

        operator bool() const
        {
            return !is_null;
        }

        operator const T&() const
        {
            return std::move(value);
        }

        friend bool operator==(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value == rhs;
        }

        friend bool operator!=(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value != rhs;
        }

        friend bool operator<(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value < rhs;
        }

        friend bool operator<=(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value <= rhs;
        }

        friend bool operator>(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value > rhs;
        }

        friend bool operator>=(const nullable<T>& lhs, const T& rhs)
        {
            return lhs.value >= rhs;
        }

        friend bool operator==(const T& lhs, const nullable<T>& rhs)
        {
            return lhs == rhs.value;
        }

        friend bool operator!=(const T& lhs, const nullable<T>& rhs)
        {
            return lhs != rhs.value;
        }

        friend bool operator<(const T& lhs, const nullable<T>& rhs)
        {
            return lhs < rhs.value;
        }

        friend bool operator<=(const T& lhs, const nullable<T>& rhs)
        {
            return lhs <= rhs.value;
        }

        friend bool operator>(const T& lhs, const nullable<T>& rhs)
        {
            return lhs > rhs.value;
        }

        friend bool operator>=(const T& lhs, const nullable<T>& rhs)
        {
            return lhs >= rhs.value;
        }

        friend bool operator==(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value == rhs.value;
        }

        friend bool operator!=(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value != rhs.value;
        }

        friend bool operator<(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value < rhs.value;
        }

        friend bool operator<=(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value <= rhs.value;
        }

        friend bool operator>(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value > rhs.value;
        }

        friend bool operator>=(const nullable<T>& lhs, const nullable<T>& rhs)
        {
            return lhs.value >= rhs.value;
        }

        friend std::ostream& operator<<(std::ostream& os, const nullable& n)
        {
            os << n.value;
            return os;
        }

    private:
        bool is_null = true;
        T value;
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_NULLABLE_ */
