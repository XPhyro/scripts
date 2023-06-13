#ifndef HEADER_SCRIPTS_CXX_VECTOR_
#define HEADER_SCRIPTS_CXX_VECTOR_

#include <array>
#include <cstddef>
#include <iostream>

template <std::size_t Td>
struct Vector final {
private:
    std::array<double, Td> data;

public:
    inline double operator[](std::size_t i) const
    {
        return this->data[i];
    }

    inline double& operator[](std::size_t i)
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

template <>
struct Vector<1> final {
private:
    static const constexpr std::size_t Td = 1;

public:
#pragma pack(push, 1)
    double x;
#pragma pack(pop)

    inline double operator[](std::size_t i) const
    {
        return *((&x) + i);
    }

    inline double& operator[](std::size_t i)
    {
        return *((&x) + i);
    }

    inline friend std::ostream& operator<<(std::ostream& os, const Vector& v)
    {
        os << '(' << v[0] << ')';
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

    VECTOR_DEFINE_OPERATOR_(+)
    VECTOR_DEFINE_OPERATOR_(-)
    VECTOR_DEFINE_OPERATOR_(*)
    VECTOR_DEFINE_OPERATOR_(/)
};

template <>
struct Vector<2> final {
private:
    static const constexpr std::size_t Td = 2;

public:
#pragma pack(push, 1)
    double x;
    double y;
#pragma pack(pop)

    inline double operator[](std::size_t i) const
    {
        return *((&x) + i);
    }

    inline double& operator[](std::size_t i)
    {
        return *((&x) + i);
    }

    inline friend std::ostream& operator<<(std::ostream& os, const Vector& v)
    {
        os << '(' << v[0] << ", " << v[1] << ')';
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

    VECTOR_DEFINE_OPERATOR_(+)
    VECTOR_DEFINE_OPERATOR_(-)
    VECTOR_DEFINE_OPERATOR_(*)
    VECTOR_DEFINE_OPERATOR_(/)
};

template <>
struct Vector<3> final {
private:
    static const constexpr std::size_t Td = 3;

public:
#pragma pack(push, 1)
    double x;
    double y;
    double z;
#pragma pack(pop)

    inline double operator[](std::size_t i) const
    {
        return *((&x) + i);
    }

    inline double& operator[](std::size_t i)
    {
        return *((&x) + i);
    }

    inline friend std::ostream& operator<<(std::ostream& os, const Vector& v)
    {
        os << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
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

    VECTOR_DEFINE_OPERATOR_(+)
    VECTOR_DEFINE_OPERATOR_(-)
    VECTOR_DEFINE_OPERATOR_(*)
    VECTOR_DEFINE_OPERATOR_(/)
};

template <>
struct Vector<4> final {
private:
    static const constexpr std::size_t Td = 4;

public:
#pragma pack(push, 1)
    double x;
    double y;
    double z;
    double w;
#pragma pack(pop)

    inline double operator[](std::size_t i) const
    {
        return *((&x) + i);
    }

    inline double& operator[](std::size_t i)
    {
        return *((&x) + i);
    }

    inline friend std::ostream& operator<<(std::ostream& os, const Vector& v)
    {
        os << '(' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ')';
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

    VECTOR_DEFINE_OPERATOR_(+)
    VECTOR_DEFINE_OPERATOR_(-)
    VECTOR_DEFINE_OPERATOR_(*)
    VECTOR_DEFINE_OPERATOR_(/)
};

#endif /* ifndef HEADER_SCRIPTS_CXX_VECTOR_ */
