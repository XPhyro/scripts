#ifndef HEADER_SCRIPTS_CXX_VECTOR_X_
#define HEADER_SCRIPTS_CXX_VECTOR_X_

#include <array>
#include <cstddef>
#include <iostream>

#define VECTOR_X_DEFINE_OPERATOR_(OP)                                 \
    template <typename T>                                             \
    inline friend vector<Tdim> operator OP(vector<Tdim> v, const T t) \
    {                                                                 \
        for (std::size_t i = 0; i < Tdim; ++i)                        \
            v[i] = v[i] OP t;                                         \
        return v;                                                     \
    }

namespace xph {
    template <std::size_t Tdim>
    struct vector final {
    private:
        std::array<double, Tdim> data;

    public:
        inline double operator[](std::size_t i) const
        {
            return this->data[i];
        }

        inline double& operator[](std::size_t i)
        {
            return this->data[i];
        }

        inline friend std::ostream& operator<<(std::ostream& os, const vector<Tdim>& v)
        {
            for (std::size_t i = 0; i < Tdim - 1; ++i)
                os << v[i] << ", ";
            os << v[Tdim - 1] << ')';
            return os;
        }

        inline friend vector<Tdim> operator+(vector<Tdim> v, const vector<Tdim>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] += vp[i];

            return v;
        }

        inline friend vector<Tdim> operator-(vector<Tdim> v, const vector<Tdim>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] -= vp[i];

            return v;
        }

        inline friend vector<Tdim> operator*(vector<Tdim> v, const vector<Tdim>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] *= vp[i];

            return v;
        }

        inline friend vector<Tdim> operator/(vector<Tdim> v, const vector<Tdim>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] /= vp[i];

            return v;
        }

        VECTOR_X_DEFINE_OPERATOR_(+)
        VECTOR_X_DEFINE_OPERATOR_(-)
        VECTOR_X_DEFINE_OPERATOR_(*)
        VECTOR_X_DEFINE_OPERATOR_(/)
    };

    template <>
    struct vector<1> final {
    private:
        static const constexpr std::size_t Tdim = 1;

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

        inline friend std::ostream& operator<<(std::ostream& os, const vector<1>& v)
        {
            os << '(' << v[0] << ')';
            return os;
        }

        inline friend vector<1> operator+(vector<1> v, const vector<1>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] += vp[i];

            return v;
        }

        inline friend vector<1> operator-(vector<1> v, const vector<1>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] -= vp[i];

            return v;
        }

        inline friend vector<1> operator*(vector<1> v, const vector<1>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] *= vp[i];

            return v;
        }

        inline friend vector<1> operator/(vector<1> v, const vector<1>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] /= vp[i];

            return v;
        }

        VECTOR_X_DEFINE_OPERATOR_(+)
        VECTOR_X_DEFINE_OPERATOR_(-)
        VECTOR_X_DEFINE_OPERATOR_(*)
        VECTOR_X_DEFINE_OPERATOR_(/)
    };

    template <>
    struct vector<2> final {
    private:
        static const constexpr std::size_t Tdim = 2;

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

        inline friend std::ostream& operator<<(std::ostream& os, const vector<2>& v)
        {
            os << '(' << v[0] << ", " << v[1] << ')';
            return os;
        }

        inline friend vector<2> operator+(vector<2> v, const vector<2>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] += vp[i];

            return v;
        }

        inline friend vector<2> operator-(vector<2> v, const vector<2>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] -= vp[i];

            return v;
        }

        inline friend vector<2> operator*(vector<2> v, const vector<2>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] *= vp[i];

            return v;
        }

        inline friend vector<2> operator/(vector<2> v, const vector<2>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] /= vp[i];

            return v;
        }

        VECTOR_X_DEFINE_OPERATOR_(+)
        VECTOR_X_DEFINE_OPERATOR_(-)
        VECTOR_X_DEFINE_OPERATOR_(*)
        VECTOR_X_DEFINE_OPERATOR_(/)
    };

    template <>
    struct vector<3> final {
    private:
        static const constexpr std::size_t Tdim = 3;

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

        inline friend std::ostream& operator<<(std::ostream& os, const vector<3>& v)
        {
            os << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
            return os;
        }

        inline friend vector<3> operator+(vector<3> v, const vector<3>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] += vp[i];

            return v;
        }

        inline friend vector<3> operator-(vector<3> v, const vector<3>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] -= vp[i];

            return v;
        }

        inline friend vector<3> operator*(vector<3> v, const vector<3>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] *= vp[i];

            return v;
        }

        inline friend vector<3> operator/(vector<3> v, const vector<3>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] /= vp[i];

            return v;
        }

        VECTOR_X_DEFINE_OPERATOR_(+)
        VECTOR_X_DEFINE_OPERATOR_(-)
        VECTOR_X_DEFINE_OPERATOR_(*)
        VECTOR_X_DEFINE_OPERATOR_(/)
    };

    template <>
    struct vector<4> final {
    private:
        static const constexpr std::size_t Tdim = 4;

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

        inline friend std::ostream& operator<<(std::ostream& os, const vector<4>& v)
        {
            os << '(' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ')';
            return os;
        }

        inline friend vector<4> operator+(vector<4> v, const vector<4>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] += vp[i];

            return v;
        }

        inline friend vector<4> operator-(vector<4> v, const vector<4>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] -= vp[i];

            return v;
        }

        inline friend vector<4> operator*(vector<4> v, const vector<4>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] *= vp[i];

            return v;
        }

        inline friend vector<4> operator/(vector<4> v, const vector<4>& vp)
        {
            for (std::size_t i = 0; i < Tdim; ++i)
                v[i] /= vp[i];

            return v;
        }

        VECTOR_X_DEFINE_OPERATOR_(+)
        VECTOR_X_DEFINE_OPERATOR_(-)
        VECTOR_X_DEFINE_OPERATOR_(*)
        VECTOR_X_DEFINE_OPERATOR_(/)
    };
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_VECTOR_X_ */
