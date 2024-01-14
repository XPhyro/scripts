#ifndef HEADER_SCRIPTS_CXX_MATHUTIL_
#define HEADER_SCRIPTS_CXX_MATHUTIL_

namespace xph {
    inline bool approx_zero(float x)
    {
        return -1e-6f < x && x < 1e-6f;
    }

    inline bool approx_zero(double x)
    {
        return -1e-12 < x && x < 1e-12;
    }

    inline bool approx_eq(float x, float y)
    {
        return approx_zero(x - y);
    }

    inline bool approx_eq(double x, double y)
    {
        return approx_zero(x - y);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_MATHUTIL_ */
