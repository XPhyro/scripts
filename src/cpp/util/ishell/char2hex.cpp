#include <iostream>

int main(const int argc, const char* argv[])
{
    for (char c; std::cin >> c;)
        std::cout << std::hex << (int)c << '\n';

    return 0;
}
