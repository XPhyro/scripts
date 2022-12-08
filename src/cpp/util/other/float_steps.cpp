// @CXXFLAGS -Wno-strict-aliasing

#include <iostream>
#include <ranges>

int main()
{
    for (const auto&& i : std::ranges::iota_view(0ull, ~0ull)) {
        float f;
        reinterpret_cast<std::uint64_t&>(f) = i;
        std::cout << f << '\n';
    }

    return 0;
}
