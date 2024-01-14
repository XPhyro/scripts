#ifndef HEADER_SCRIPTS_CXX_MATHUTIL_
#define HEADER_SCRIPTS_CXX_MATHUTIL_

namespace xph {
    inline bool approx_zero(float x, float e = 1e-6f)
    {
        return -e < x && x < e;
    }

    inline bool approx_zero(double x, double e = 1e-12)
    {
        return -e < x && x < e;
    }

    inline bool approx_eq(float x, float y, float e = 1e-6f)
    {
        return approx_zero(x - y, e);
    }

    inline bool approx_eq(double x, double y, double e = 1e-12)
    {
        return approx_zero(x - y, e);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_MATHUTIL_ */
