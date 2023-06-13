// @CXXFLAGS -Wno-strict-aliasing

#include <cstdint>
#include <iostream>
#include <ranges>

int main()
{
    for (const auto&& i : std::ranges::iota_view(0ull, ~0ull)) {
        double d;
        reinterpret_cast<std::uint64_t&>(d) = i;
        std::cout << d << '\n';
    }

    return 0;
}
