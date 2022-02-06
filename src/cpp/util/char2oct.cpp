#include <iostream>

int main(const int argc, const char *argv[])
{
    for (char c; std::cin >> c;)
        std::cout << std::oct << (int)c << '\n';

    return 0;
}
