// C++
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// C
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

// C++ libraries
#include <strutil.hpp>

// C libraries
#include <strutil.h>
#include <sysutil.h>

// third party
#include <hedley.h>

typedef enum {
    CACHE_TEMPORARY,
    CACHE_PERSISTENT,
} cache_t;

std::pair<size_t, std::string> readcache(std::string flname);
HEDLEY_NO_RETURN void die(const std::string err);

const std::unordered_map<std::string, cache_t> caches = {
    { "t", CACHE_TEMPORARY },    { "tmp", CACHE_TEMPORARY },
    { "temp", CACHE_TEMPORARY }, { "temporary", CACHE_TEMPORARY },
    { "p", CACHE_PERSISTENT },   { "persistent", CACHE_PERSISTENT },
};
const char* const givenexecname = "shvector";
const char* execname;

int main(int argc, char* argv[])
{
    std::string proghash, arrname;
    const std::vector<std::string> v;
    cache_t cache = CACHE_TEMPORARY;
    int i;

    if (!argc)
        execname = "NULL";
    else
        execname = argv[0];

    while ((i = getopt(argc, argv, "c:h")) != -1) {
        switch (i) {
            case 'c':
                try {
                    cache = caches.at(strutil::makelower(optarg));
                } catch (const std::out_of_range& e) {
                    std::cerr << execname << ": unkown cache type " << optarg << '\n';
                    std::exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                std::cout
                    << "Usage: " << execname
                    << " [OPTION...] [PROG_HASH] [SYNTAX]\n"
                       "Handle arrays in a strictly POSIX shell.\n"
                       "\n"
                       "SYNTAX may be one of:\n"
                       "1. [ARR_NAME]\n"
                       "   1. Get array.\n"
                       "   2. Array must have been initialised.\n"
                       "2. [ARR_NAME] [INDEX]\n"
                       "   1. Get value at INDEX.\n"
                       "   2. INDEX must be in range [0, size).\n"
                       "   3. Array must have been initialised.\n"
                       "3. [ARR_NAME] = [OTHER_ARR_NAME]\n"
                       "   1. Copy OTHER_ARR_NAME to ARR_NAME.\n"
                       "   2. If OTHER_ARR_NAME is NULL or nullptr, uninitialise array.\n"
                       "4. [ARR_NAME] [INDEX] = [VALUE]\n"
                       "   1. Set value at INDEX to VALUE.\n"
                       "   2. INDEX must be in range [0, size).\n"
                       "   3. Array must have been initialised.\n"
                       "5. [ARR_NAME] [FUNC]\n"
                       "   1. new\n"
                       "      1. Initialise array.\n"
                       "      2. If already initialised, array is reinitialised.\n"
                       "   2. size\n"
                       "      1. Get array size.\n"
                       "      2. Array must have been initialised.\n"
                       "   3. push_back [VALUE]\n"
                       "      1. Append VALUE to the end of the array.\n"
                       "      2. Array must have been initialised.\n"
                       "\n"
                       "Options\n"
                       "  -c     set cache type. must be one of {{t, tmp, temp, temporary}, {p, persistent}}. default is temporary.\n"
                       "  -h     display this help and exit\n";
                std::exit(EXIT_SUCCESS);
            default:
                std::cerr << "Try '" << execname << " -h' for more information.\n";
                std::exit(EXIT_FAILURE);
        }
    }

    argv += optind;
    argc -= optind;

    if (argc < 2)
        die("invalid syntax");

    proghash = argv[0];
    arrname = argv[1];
    argv += 2;
    argc -= 2;

    const char* prefix;
    switch (cache) {
        case CACHE_TEMPORARY:
            if (!(prefix = std::getenv("TMPDIR")))
                prefix = "/tmp";
            break;
        case CACHE_PERSISTENT:
            if (!(prefix = std::getenv("XDG_CACHE_HOME"))) {
                if (!(prefix = getenv("HOME")) && !(prefix = getpwuid(getuid())->pw_dir))
                    die("could not determine persistent cache directory");
                std::ostringstream ss;
                ss << prefix << "/.cache";
                prefix = ss.str().c_str();
            }
            break;
        default:
            die("unkown cache type");
    }

    std::string cachedir;
    std::string cachefl;
    {
        std::ostringstream ss;
        ss << prefix << '/' << givenexecname << '/' << proghash;
        cachedir = ss.str();
        ss << '/' << arrname;
        cachefl = ss.str();
    }

    rmkdirconst(cachedir.c_str(), 0755);

    switch (argc) {
        case 0: {
            auto cache = readcache(cachefl);
            auto size = cache.first;
            auto buf = cache.second;

            for (auto&& view :
                 buf | std::views::split('\0') | std::views::transform([](auto&& r) -> std::string {
                     return { &*r.begin(),
                              static_cast<std::string::size_type>(std::ranges::distance(r)) };
                 }) | std::views::take(size)) {
                std::cout << view << '\0';
            }
        } break;
        case 1: {
            if (streq(argv[0], "new")) {
                std::ofstream fl(cachefl, std::ios::out | std::ios::binary);

                if (!fl)
                    die("could not open cache file");

                size_t size = 2;
                fl.write(reinterpret_cast<char*>(&size), sizeof(size));
                fl.write("\0", 1);

                fl.close();
            } else if (streq(argv[0], "size")) {
                auto cache = readcache(cachefl);
                auto size = cache.first;
                std::cout << size;
            } else {
                // TODO: if parseable to int, index
            }
        } break;
        case 2: {
            if (streq(argv[0], "=")) {
                // TODO: copy
            } else if (streq(argv[0], "push_back")) {
                // TODO: append
            }
        } break;
        case 3: {
            // TODO: if argv[0] is parseable to int and argv[1] is =, set index
        } break;
        default: {
        } break;
    }

    return EXIT_SUCCESS;
}

HEDLEY_NO_RETURN void die(const std::string err)
{
    std::cerr << execname << ": " << err << '\n';
    std::exit(EXIT_FAILURE);
}

std::pair<size_t, std::string> readcache(std::string flname)
{
    std::ifstream fl(flname, std::ios::in | std::ios::binary);

    if (!fl)
        die("could not open cache file");

    size_t size;
    fl.read(reinterpret_cast<char*>(&size), sizeof(size));
    fl.seekg(1, std::ios::cur);

    std::string buf;
    {
        std::ostringstream ss;
        ss << fl.rdbuf();
        fl.close();
        buf = ss.str();
    }

    return { size, buf };
}
