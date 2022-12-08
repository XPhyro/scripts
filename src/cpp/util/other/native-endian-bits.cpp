#include <bitset>
#include <iostream>
#include <ranges>

int main()
{
    for (const auto&& i : std::ranges::iota_view(0ull, ~0ull))
        std::cout << std::bitset<sizeof(i) * 8>(i) << '\n';

    return 0;
}
