// @CXXFLAGS -Wno-strict-aliasing -Wno-uninitialized

#include <cstdint>
#include <iostream>

int main()
{
    double d;

    static_assert(sizeof(double) == sizeof(std::uint64_t));
    reinterpret_cast<std::uint64_t&>(d) = 1;

    std::cout << d << '\n';

    return 0;
}
