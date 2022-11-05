#include <bitset>
#include <cstdint>
#include <iostream>
#include <limits>

int main()
{
    for (std::uintmax_t i = 0; i < std::numeric_limits<std::uintmax_t>::max(); ++i)
        std::cout << std::bitset<sizeof(std::uintmax_t) * 8>(i) << '\n';

    return 0;
}
