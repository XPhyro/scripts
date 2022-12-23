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
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_MATHUTIL_ */
