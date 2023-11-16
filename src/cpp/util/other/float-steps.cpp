// @CXXFLAGS -Wno-strict-aliasing

#include <cstdint>
#include <iostream>
#include <ranges>

int main()
{
    for (const auto&& i : std::ranges::iota_view(0ull, ~0ull)) {
        float f;
        reinterpret_cast<std::uint32_t&>(f) = i;
        std::cout << f << '\n';
    }

    return 0;
}
