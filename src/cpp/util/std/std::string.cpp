#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <unistd.h>

#include <die.hpp>
#include <exec_info.hpp>
#include <macros.hpp>
#include <strutil.hpp>

DEFINE_EXEC_INFO();

std::string get_input(int& argc, char**& argv);
void align(int& argc, char**& argv);
void replace_all(int& argc, char**& argv);

std::unordered_map<std::string, std::function<void(int&, char**&)>> functions = {
    { "align", align },
    { "replace_all", replace_all },
};

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    if (argc < 2)
        xph::die("no function given");
    std::string requested_function_name = argv[1];

    argc -= 2;
    argv += 2;

    for (const auto& [function_name, function] : functions) {
        if (function_name == requested_function_name) {
            function(argc, argv);
            return EXIT_SUCCESS;
        }
    }

    xph::die("unkown function: ", requested_function_name);

    return EXIT_SUCCESS;
}

std::string get_input(int& argc, char**& argv)
{
    std::stringstream ss;

    // TODO: don't load whole strings into memory, process by chunks.
    if (!argc) {
        ss << std::cin.rdbuf();
    } else {
        for (const auto& arg : std::views::counted(argv, argc - 1)) {
            ss << arg;
            ss << '\n';
        }
        ss << argv[argc - 1];
    }

    return ss.str();
}

void align(int& argc, char**& argv)
{
    if (argc < 1)
        xph::die("insufficient number of operands given");

    if (!*argv[0])
        xph::die("alignment string cannot be empty");

    std::string align = argv[0];
    --argc;
    ++argv;

    std::vector<std::size_t> max_positions;
    std::vector<std::vector<std::size_t>> positions;
    std::vector<std::string> lines;

    {
        std::size_t i = 0;
        auto input = get_input(argc, argv);
        for (const auto& line : input | xph::str::splitview('\n')) {
            positions.push_back({});
            lines.push_back({ line });

            auto pos = line.find(align, 0);
            std::size_t j = 0;

            while (pos != std::string::npos) {
                positions[i].push_back(pos);

                if (max_positions.size() < j + 1)
                    max_positions.push_back(pos);
                else if (pos > max_positions[j])
                    max_positions[j] = pos;

                pos = line.find(align, pos + 1);

                ++j;
            }

            ++i;
        }
    }

    for (auto&& it = max_positions.begin() + 1; it != max_positions.end(); ++it) {
        if (*it < *(it - 1))
            *it = *(it - 1) + 1;
    }

    for (std::size_t l = 0; l < lines.size(); ++l) {
        const auto& line = lines[l];

        if (auto& line_positions = positions[l]; line_positions.empty()) {
            std::cout << line << '\n';
        } else {
            line_positions.reserve(line_positions.size() + 1); // force small allocation
            line_positions.push_back(line.size());

            std::size_t current_pos = 0;

            if (auto first_pos = line_positions.front(); first_pos != 0) {
                std::cout << std::string_view(line.begin(), line.begin() + first_pos);
                current_pos += first_pos;
            }

            for (std::size_t lp = 0; lp < line_positions.size() - 1; ++lp) {
                auto pos = line_positions[lp];
                auto next_pos = line_positions[lp + 1];
                auto max_pos = max_positions[lp];

                if (max_pos > current_pos) {
                    for ([[maybe_unused]] const auto&& _ :
                         std::views::iota(0u, max_pos - current_pos))
                        std::cout << ' ';
                }
                std::cout << std::string_view(line.begin() + pos, line.begin() + next_pos);
                current_pos += max_positions[lp] - current_pos - 1 + next_pos - pos;
            }

            std::cout << '\n';
        }
    }
}

void replace_all(int& argc, char**& argv)
{
    if (argc < 2)
        xph::die("insufficient number of operands given");

    if (!*argv[0])
        xph::die("from string cannot be empty");

    std::string from = argv[0], to = argv[1];
    argc -= 2;
    argv += 2;

    auto input = get_input(argc, argv);

    xph::str::replaceall(input, from, to);
    std::cout << input;
}
