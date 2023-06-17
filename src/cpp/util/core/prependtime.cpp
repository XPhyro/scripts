#include <chrono>
#include <iostream>
#include <string>

#include <unitutil.hpp>

int main()
{
    std::chrono::steady_clock clock;

    auto start_time = clock.now();
    auto last_time = start_time;

    for (std::string line; std::getline(std::cin, line);) {
        auto current_time = clock.now();

        auto elapsed_time = current_time - start_time;
        auto elapsed_ticks = elapsed_time.count();
        auto elapsed_human_ticks = xph::unit::tick_to_human(elapsed_ticks);

        auto differential_time = current_time - last_time;
        auto differential_ticks = differential_time.count();
        auto differential_human_ticks = xph::unit::tick_to_human(differential_ticks);

        std::cout << '[' << elapsed_human_ticks << " (+" << differential_human_ticks
                  << ")]: " << line << '\n';

        last_time = current_time;
    }

    return 0;
}
