#ifndef HEADER_SCRIPTS_CXX_VECTOR_
#define HEADER_SCRIPTS_CXX_VECTOR_

#include <array>
#include <cstddef>
#include <iostream>

template <std::size_t Td>
struct Vector final {
public:
    std::array<double, Td> data;

    inline double operator[](std::size_t i)
    {
        return this->data[i];
    }

    inline friend std::ostream& operator<<(std::ostream& os, const Vector& v)
    {
        for (std::size_t i = 0; i < Td - 1; ++i)
            os << v[i] << ", ";
        os << v[Td - 1] << ')';
        return os;
    }

    inline friend Vector operator+(Vector v, Vector vp)
    {
        for (std::size_t i = 0; i < Td; ++i)
            v[i] += vp[i];

        return v;
    }

    inline friend Vector operator-(Vector v, Vector vp)
    {
        for (std::size_t i = 0; i < Td; ++i)
            v[i] -= vp[i];

        return v;
    }

    inline friend Vector operator*(Vector v, Vector vp)
    {
        for (std::size_t i = 0; i < Td; ++i)
            v[i] *= vp[i];

        return v;
    }

    inline friend Vector operator/(Vector v, Vector vp)
    {
        for (std::size_t i = 0; i < Td; ++i)
            v[i] /= vp[i];

        return v;
    }

#define VECTOR_DEFINE_OPERATOR_(OP)                       \
    template <typename T>                                 \
    inline friend Vector operator OP(Vector v, const T t) \
    {                                                     \
        for (std::size_t i = 0; i < Td; ++i)              \
            v[i] = v[i] OP t;                             \
        return v;                                         \
    }

    VECTOR_DEFINE_OPERATOR_(+)
    VECTOR_DEFINE_OPERATOR_(-)
    VECTOR_DEFINE_OPERATOR_(*)
    VECTOR_DEFINE_OPERATOR_(/)
};

#endif /* ifndef HEADER_SCRIPTS_CXX_VECTOR_ */
