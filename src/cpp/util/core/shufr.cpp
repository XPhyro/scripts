#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

#include <die.hpp>
#include <nullable.hpp>
#include <parse.hpp>

void parseargs(int& argc, char**& argv);

char optdelim = '\n';
bool optargisline = false, optuniquemax = false;
xph::nullable<std::size_t> optrangelow, optrangehigh, optcount, optunique;

const char* execname;

int main(int argc, char* argv[])
{
    CAPTURE_EXECNAME();

    parseargs(argc, argv);

    std::vector<std::string> lines;
    if (optargisline) {
        lines.reserve(argc);
        for (const auto& arg : std::views::counted(argv, argc))
            lines.emplace_back(arg);
    } else if (optrangelow && optrangehigh) {
        for (std::ostringstream ss;
             const auto& i : std::views::iota(static_cast<std::size_t>(optrangelow),
                                              static_cast<std::size_t>(optrangehigh) + 1ul)) {
            ss << i;
            lines.push_back(ss.str());
            ss.str("");
        }
    } else if (argc) {
        for (const auto& path : std::views::counted(argv, argc)) {
            std::ifstream ifs(path);
            for (std::string line; std::getline(ifs, line, optdelim); lines.push_back(line)) {}
        }
    } else {
        for (std::string line; std::getline(std::cin, line, optdelim); lines.push_back(line)) {}
    }

    if (lines.empty())
        xph::die("no lines to repeat");

    if (optuniquemax)
        optunique = lines.size();
    else if (optunique && optunique > lines.size())
        xph::die("not enough lines to ensure unique sequences of size ", optunique);

    std::random_device rdev;
    std::mt19937 rng(rdev());

    if (!optunique || optunique <= 1ul) {
        std::uniform_int_distribution<std::size_t> dist(0u, lines.size() - 1);
        for (std::size_t count = 0; !optcount || count < optcount;
             std::cout << lines[dist(rng)] << optdelim, ++count) {}
    } else {
        std::vector<std::size_t> index_turns(lines.size());
        std::set<std::size_t> inactive_indices;
        std::set<std::size_t> inactive_indices_buffer;
        std::vector<std::size_t> active_indices;
        active_indices.reserve(lines.size());
        for (const auto&& i : std::views::iota(0u, lines.size()))
            active_indices.push_back(i);

        for (std::size_t count = 0; !optcount || count < optcount; ++count) {
            for (const auto& index : inactive_indices) {
                if (!--index_turns[index]) {
                    inactive_indices_buffer.insert(index);
                    active_indices.push_back(index);
                }
            }

            for (const auto& index : inactive_indices_buffer)
                inactive_indices.erase(index);
            inactive_indices_buffer.clear();

            std::uniform_int_distribution<std::size_t> dist(0u, active_indices.size() - 1);
            auto r = dist(rng);
            auto i = active_indices[r];
            active_indices.erase(active_indices.begin() + r);

            index_turns[i] = optunique;
            inactive_indices.insert(i);

            std::cout << lines[i] << optdelim;
        }
    }

    return EXIT_SUCCESS;
}

void parseargs(int& argc, char**& argv)
{
    int i;

    while ((i = getopt(argc, argv, "ehi:n:rUu:z0")) != -1) {
        switch (i) {
            case 'e':
                optargisline = true;
                break;
            case 'h':
                std::cout
                    << "Usage: " << execname
                    << " [OPTION]... [FILE]...\n"
                       "       "
                    << execname
                    << " -e [OPTION]... [ARG]...\n"
                       "       "
                    << execname
                    << " -i LO-HI [OPTION]...\n"
                       "Repeatedly write a random permutation of the input lines to standard output.\n"
                       "\n"
                       "With no FILE, or when file is /dev/stdin, read standard input.\n"
                       "\n"
                       "This program is not suitable for cryptographic use.\n"
                       "\n"
                       "  -e        treat each ARG as an input line\n"
                       "  -h        display this help and exit\n"
                       "  -i LO-HI  treat each number LO through HI as an input line. LO must be less than HI. if HI is negative, it is interpreted as ("
                    << std::numeric_limits<std::size_t>::max() << " - (|HI| - 1)).\n"
                    << "  -n COUNT  output at most COUNT lines\n"
                       "  -r        ignored. provided for partial compatibility with GNU shuf.\n"
                       "  -U        alias for `-u $line_count`. overrides -u.\n"
                       "  -u COUNT  make sure any sequence of COUNT lines have unique indices. if lines are unique, the output lines will also be unique. COUNT must be less than or equal to the count of lines provided via stdin.\n"
                       "  -z        line delimiter is NUL, not newline\n"
                       "  -0        line delimiter is NUL, not newline\n";
                std::exit(EXIT_SUCCESS);
            case 'i': {
                std::string_view arg(optarg);
                const auto idx = arg.find('-');
                const auto low = arg.substr(0, idx + 1);
                const auto high = arg.substr(idx + 1);
                optrangelow = xph::parse<std::size_t>(low);
                optrangehigh = xph::parse<std::size_t>(high);
            } break;
            case 'n':
                optcount = xph::parse<std::size_t>(optarg);
                break;
            case 'r':
                break;
            case 'U':
                optuniquemax = true;
                break;
            case 'u':
                optunique = xph::parse<std::size_t>(optarg);
                break;
            case 'z':
            case '0':
                optdelim = '\0';
                break;
            default:
                xph::die("Try '", execname, " -h' for more information.");
        }
    }

    argc -= optind;
    argv += optind;

    if (optrangelow || optrangehigh) {
        if (optargisline)
            xph::die("cannot combine -e and -i options");
        if (argc)
            xph::die(
                "extra operand ‘", argv[0], "‘\nTry '", execname, " -h' for more information.");
    }
    if (optrangelow > optrangehigh)
        xph::die("LO must be less than HI");
}
