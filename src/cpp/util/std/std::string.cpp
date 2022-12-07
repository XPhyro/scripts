#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>

#include <unistd.h>

#include <die.hpp>
#include <exec_info.hpp>
#include <macros.hpp>
#include <strutil.hpp>

DEFINE_EXEC_INFO();

void replace_all(int& argc, char**& argv);

std::unordered_map<std::string, std::function<void(int&, char**&)>> functions = {
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

void replace_all(int& argc, char**& argv)
{
    if (argc < 2)
        xph::die("insufficient number of operands given");

    if (!*argv[0])
        xph::die("from string cannot be empty");

    std::string from = argv[0], to = argv[1];
    argc -= 2;
    argv += 2;

    std::string str; // TODO: optimise

    if (argc > 0) {
        std::stringstream ss;
        for (const auto& arg : std::views::counted(argv, argc - 1)) {
            ss << arg;
            ss << ' ';
        }
        ss << argv[argc - 1];
        str = ss.str();
    } else {
        std::stringstream ss;
        ss << std::cin.rdbuf();
        str = ss.str();
    }

    xph::str::replaceall(str, from, to);
    std::cout << str;
}
