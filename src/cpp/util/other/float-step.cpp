// @CXXFLAGS -Wno-strict-aliasing -Wno-uninitialized

#include <cstdint>
#include <iostream>

int main()
{
    float f;

    static_assert(sizeof(float) == sizeof(std::uint32_t));
    reinterpret_cast<std::uint32_t&>(f) = 1;

    std::cout << f << '\n';

    return 0;
}
