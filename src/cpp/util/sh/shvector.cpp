#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

#include <strutil.hpp>

typedef enum {
    CACHE_TEMPORARY,
    CACHE_PERSISTENT,
} cache_t;

int main(int argc, char* argv[])
{
    const char* const execname = argv[0];
    const std::vector<std::string> v;
    cache_t cache;
    int i;

    while ((i = getopt(argc, argv, "ch"))) {
        switch (i) {
            case 'c':
                if (auto caches = strutil::makelower(optarg);
                    caches == "t" || caches == "tmp" || caches == "temp" || caches == "temporary")
                    cache = CACHE_TEMPORARY;
                else if (caches == "p" || caches == "persistent")
                    cache = CACHE_PERSISTENT;
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
                       "  -c     set cache type. must be one of {{t, tmp, temp, temporary}, {p, persistent}}. default is temporary.\n";
                std::exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                std::exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    // TODO: array structure should be:
    // size\0
    // elem 0\0
    // elem 1\0
    // ...
    // elem size - 1\0
    // debug data\0
    // TODO: elements shold be stored as strings.

    return 0;
}
