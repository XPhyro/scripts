// C++
#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// C
#include <unistd.h>

// libraries
#include <consts.hpp>
#include <macros.hpp>
#include <strutil.hpp>

// third party
#include <hedley.h>

DEFINE_ENUM(type, character COMMA string COMMA octal COMMA decimal COMMA hexadecimal COMMA integer
                      COMMA binary COMMA floating_point COMMA raw_binary COMMA);

HEDLEY_NO_RETURN void help();
HEDLEY_NO_RETURN void invalidargs(std::string err);
HEDLEY_NO_RETURN void invalidcast();
void castint(std::string val);
void castfloat(std::string val);
void caststr(std::string val);
void castchar(std::string val);

const std::unordered_map<std::string, type> types = {
    { "char", type::character }, { "str", type::string },           { "oct", type::octal },
    { "dec", type::decimal },    { "hex", type::hexadecimal },      { "int", type::integer },
    { "bin", type::binary },     { "float", type::floating_point }, { "raw", type::raw_binary },
};

std::string execname, fromtypename, totypename;
type fromtype, totype;

int main(int argc, char* argv[])
{
    execname = argv[0];

    for (int i; (i = getopt(argc, argv, "h")) != -1;) {
        switch (i) {
            case 'h':
                help();
            default:
                invalidargs(consts::str::empty);
        }
    }

    argv += optind;
    argc -= optind;

    if (argc < 2)
        invalidargs(consts::str::empty);

    try {
        fromtype = types.at(fromtypename = strutil::makelower(std::string(argv[0])));
        totype = types.at(totypename = strutil::makelower(std::string(argv[1])));
    } catch (const std::out_of_range& e) {
        invalidargs("invalid type given");
    }

    argv += 2;
    argc -= 2;

    std::function<void(std::string)> func;
    switch (fromtype.value()) {
        case type::character:
            func = castchar;
            break;
        case type::string:
            func = caststr;
            break;
        case type::floating_point:
            func = castfloat;
            break;
        default:
            func = castint;
            break;
    }

    if (argc) {
        auto args = std::views::counted(argv, argc);
        std::ranges::for_each(args.begin(), args.end(), func);
    } else {
        for (std::string s; std::cin >> s;)
            func(s);
    }
}

HEDLEY_NO_RETURN void help()
{
    std::cout << "Usage: " << execname
              << " [OPTION...] [FROM_TYPE] [TO_TYPE] [VALUE...]\n"
                 "Cast types to types.\n"
                 "\n"
                 "Valid types are: ";

    std::vector<std::string> typenames;
    typenames.reserve(types.size());
    for (const auto& [name, _] : types)
        typenames.push_back(name);

    std::sort(typenames.begin(), typenames.end());

    for (auto it = typenames.begin(); it < typenames.end() - 1; it++)
        std::cout << *it << ", ";
    std::cout << typenames.back()
              << ".\n"
                 "\n"
                 "  -h        display this help and exit\n";

    std::exit(EXIT_SUCCESS);
}

HEDLEY_NO_RETURN void invalidargs(const std::string err)
{
    if (!err.empty())
        std::cerr << err << '\n';
    std::cerr << "Try '" << execname << " -h' for more information.\n";
    std::exit(EXIT_FAILURE);
}

HEDLEY_NO_RETURN void invalidcast()
{
    std::cerr << execname << ": given cast of `" << fromtypename << "` to `" << totypename
              << "` is not supported.\n";
    std::exit(EXIT_FAILURE);
}

void castint(std::string val)
{
    static std::stringstream ss;
    long long ll;

    ss.str("");
    ss.clear();

    ss << val;
    ss >> ll;

    switch (totype.value()) {
        case type::character:
            std::cout << static_cast<char>(ll);
            break;
        case type::string:
            std::cout << static_cast<char>(ll) << '\n';
            break;
        case type::octal:
            std::cout << std::oct << ll << '\n';
            break;
        case type::decimal:
        case type::integer:
            std::cout << std::dec << ll << '\n';
            break;
        case type::hexadecimal:
            std::cout << std::hex << ll << '\n';
            break;
        case type::binary:
            std::cout << std::bitset<8 * sizeof(ll)>(ll);
            break;
        case type::raw_binary:
            write(STDOUT_FILENO, &ll, sizeof(ll));
            break;
        default:
            invalidcast();
    }
}

void castfloat(std::string val)
{
    static std::stringstream ss;
    double f;

    ss.str("");
    ss.clear();

    ss << val;
    ss >> f;

    switch (totype.value()) {
        case type::raw_binary:
            write(STDOUT_FILENO, &f, sizeof(f));
            break;
        default:
            invalidcast();
    }
}

void caststr(std::string val)
{
    static unsigned char bits = 0;
    static int bitidx = 0;

    switch (totype.value()) {
        case type::character:
            castchar(val);
            break;
        case type::string:
            std::cout << val << '\n';
            break;
        case type::binary:
            castchar(val);
            std::cout << '\n';
            break;
        case type::raw_binary: {
            std::string_view view{ val };
            for (size_t i = 0; i < view.length(); i += 8) {
                for (auto const& c : view.substr(i, 8) | std::views::reverse) {
                    switch (c) {
                        case '0':
                            bits &= ~(static_cast<unsigned char>(1) << bitidx++);
                            break;
                        case '1':
                            bits |= static_cast<unsigned char>(1) << bitidx++;
                            break;
                        default:
                            std::cerr << execname << ": expected 0 or 1, got '" << c << "'.\n";
                            std::exit(EXIT_FAILURE);
                    }
                    if (bitidx == 8) {
                        bitidx = 0;
                        std::cout << bits;
                    }
                }
            }
        } break;
        default:
            invalidcast();
    }
}

void castchar(std::string val)
{
    for (auto const& c : val) {
        switch (totype.value()) {
            case type::character:
                std::cout << c;
                break;
            case type::string:
                std::cout << c << '\n';
                break;
            case type::octal:
                std::cout << std::oct << static_cast<int>(c) << '\n';
                break;
            case type::decimal:
            case type::integer:
                std::cout << std::dec << static_cast<int>(c) << '\n';
                break;
            case type::hexadecimal:
                std::cout << std::hex << static_cast<int>(c) << '\n';
                break;
            case type::binary:
                std::cout << std::bitset<8>(c);
                break;
            default:
                invalidcast();
        }
    }
}