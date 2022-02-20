#include <iostream>

int main(const int argc, const char* argv[])
{
    for (int i; std::cin >> i;)
        std::cout << (char)i << '\n';

    return 0;
}
