// C++
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
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
#include <macros.hpp>
#include <strutil.hpp>
#include <vecutil.hpp>

// C libraries
#include <strutil.h>

// third party
#include <hedley.h>

DEFINE_ENUM(cache, temporary COMMA persistent COMMA);

typedef std::size_t vecsize_t;

HEDLEY_NO_RETURN void die(const std::string& err);
cache parseargs(int* argc, char** argv[]);
void conditionaldelim();
std::pair<vecsize_t, std::string> readcache();
void writecache(std::vector<std::string> vec);
vecsize_t readsize();
void vecout();
void vecnew();
void vecsize();
void vecpopback(const std::string&& countstr);
void vecpushback(const std::string&& value);
void vecgetindex(const std::string&& indexstr);
void vecsetindex(const std::string&& indexstr, const std::string&& value);
void vecset(const std::string&& other);
void vecswap(const std::string&& other);

const std::unordered_map<std::string, cache> caches = {
    { "t", cache::temporary },    { "tmp", cache::temporary },
    { "temp", cache::temporary }, { "temporary", cache::temporary },
    { "p", cache::persistent },   { "persistent", cache::persistent },
};
const char indelim = '\0';
const auto constexpr vecview = strutil::splitview(indelim);
const std::string givenexecname = "shvector";

std::string execname, proghash, vecname, cachefl;
char outdelim = indelim;
const char* prefix;

int main(int argc, char* argv[])
{
    if (!argc)
        execname = "NULL";
    else
        execname = argv[0];

    const auto cache = parseargs(&argc, &argv);

    if (argc < 2)
        die("invalid syntax");

    proghash = argv[0];
    vecname = argv[1];
    argv += 2;
    argc -= 2;

    if (!proghash.length() || proghash == "NULL" || proghash == "nullptr")
        die("PROG_HASH cannot be \"NULL\", \"nullptr\" or empty");
    if (proghash.contains('/'))
        die("PROG_HASH cannot contain '/'");
    if (!vecname.length() || vecname == "NULL" || vecname == "nullptr")
        die("VEC_NAME cannot be \"NULL\", \"nullptr\" or empty");
    if (vecname.contains('/'))
        die("VEC_NAME cannot contain '/'");

    switch (cache.value()) {
        case cache::temporary:
            if (!(prefix = std::getenv("TMPDIR")))
                prefix = "/tmp";
            break;
        case cache::persistent:
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

    {
        std::string cachedir;
        {
            std::ostringstream ss;
            ss << prefix << '/' << givenexecname << '/' << proghash;
            cachedir = ss.str();

            if (argc == 1 && vecname == "=" &&
                (streq(argv[0], "NULL") || streq(argv[0], "nullptr"))) {
                std::filesystem::remove_all(cachedir);
                std::exit(EXIT_SUCCESS);
            }

            ss << '/' << vecname;
            cachefl = ss.str();
        }

        std::filesystem::create_directories(cachedir);
    }

    if (argc && streq(argv[0], "push_back")) {
        for (auto&& val : std::views::counted(argv + 1, argc - 1))
            vecpushback(val);
    } else {
        switch (argc) {
            case 0:
                vecout();
                break;
            case 1:
                if (streq(argv[0], "new"))
                    vecnew();
                else if (streq(argv[0], "size"))
                    vecsize();
                else if (streq(argv[0], "pop_back"))
                    vecpopback("1");
                else
                    vecgetindex(argv[0]);
                break;
            case 2:
                if (streq(argv[0], "="))
                    vecset(argv[1]);
                else if (streq(argv[0], "pop_back"))
                    vecpopback(argv[1]);
                else if (streq(argv[0], "swap"))
                    vecswap(argv[1]);
                break;
            case 3:
                if (streq(argv[1], "="))
                    vecsetindex(argv[0], argv[2]);
                break;
            default:
                die("unkown syntax");
        }
    }

    return EXIT_SUCCESS;
}

HEDLEY_NO_RETURN void die(const std::string& err)
{
    std::cerr << execname << ": " << err << '\n';
    std::exit(EXIT_FAILURE);
}

cache parseargs(int* argc, char** argv[])
{
    cache cache = cache::temporary;
    int i;
    bool optoutdelim = false;

    while ((i = getopt(*argc, *argv, "c:hnz0")) != -1) {
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
                    << "Usage: "
                    << execname
                    // TODO: Have more natural syntax.
                    // TODO: Add at, front, back, empty, clear, insert, erase, swap
                    << " [OPTION...] [PROG_HASH] [SYNTAX]\n"
                       "Handle vectors in a strictly POSIX shell.\n"
                       "\n"
                       "SYNTAX may be one of:\n"
                       "1. = {NULL, nullptr}\n"
                       "   1. Uninitialise all vectors belonging to PROG_HASH.\n"
                       "2. [VEC_NAME]\n"
                       "   1. Get vector.\n"
                       "   2. Vector must have been initialised.\n"
                       "3. [VEC_NAME] [INDEX]\n"
                       "   1. Get value at INDEX.\n"
                       "   2. INDEX must be in range [0, size).\n"
                       "   3. Vector must have been initialised.\n"
                       "4. [VEC_NAME] = [OTHER_VEC_NAME]\n"
                       "   1. Copy OTHER_VEC_NAME to VEC_NAME.\n"
                       "   2. If OTHER_VEC_NAME is NULL or nullptr, uninitialise vector. Otherwise, OTHER_VEC_NAME must have been initialised.\n"
                       "5. [VEC_NAME] [INDEX] = [VALUE]\n"
                       "   1. Set value at INDEX to VALUE.\n"
                       "   2. INDEX must be in range [0, size).\n"
                       "   3. Vector must have been initialised.\n"
                       "6. [VEC_NAME] [FUNC]\n"
                       "   1. new\n"
                       "      1. Initialise vector.\n"
                       "      2. If already initialised, vector is reinitialised.\n"
                       "   2. size\n"
                       "      1. Get vector size.\n"
                       "      2. Vector must have been initialised.\n"
                       "   3. push_back [VALUE...]\n"
                       "      1. Append VALUEs to the end of the vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   4. pop_back [COUNT]?\n"
                       "      1. Pop COUNT values from the end of the vector.\n"
                       "      2. If COUNT is not given, COUNT is 1.\n"
                       "      3. Vector must have been initialised.\n"
                       "   5. swap [OTHER_VEC_NAME]\n"
                       "      1. Swap VEC_NAME and OTHER_VEC_NAME.\n"
                       "      2. OTHER_VEC_NAME cannot be NULL or nullptr."
                       "      3. Vectors must have been initialised.\n"
                       "\n"
                       "PROG_HASH cannot be \"NULL\", \"nullptr\" or empty, or contain '/'.\n"
                       "VEC_NAME cannot be \"NULL\", \"nullptr\" or empty, or contain '/'.\n"
                       "\n"
                       "Output delimiter is newline if stdout is a tty and null otherwise.\n"
                       "\n"
                       "Options\n"
                       "  -c TYPE   set cache type. must be one of {{t, tmp, temp, temporary}, {p, persistent}}. default is temporary.\n"
                       "  -h        display this help and exit\n"
                       "  -n        force output delimiter to be newline (\\n)\n"
                       "  -z        force output delimiter to be null (\\0)\n"
                       "  -0        force output delimiter to be null (\\0)\n";
                std::exit(EXIT_SUCCESS);
            case 'n':
                optoutdelim = true;
                outdelim = '\n';
                break;
            case 'z':
            case '0':
                optoutdelim = true;
                outdelim = '\0';
                break;
            default:
                std::cerr << "Try '" << execname << " -h' for more information.\n";
                std::exit(EXIT_FAILURE);
        }
    }

    *argv += optind;
    *argc -= optind;

    if (!optoutdelim && isatty(fileno(stdout)))
        outdelim = '\n';

    return cache;
}

void assertexists(std::string path = cachefl)
{
    if (!std::filesystem::exists(path))
        die("vector is not initialised");
}

void conditionaldelim()
{
    if (outdelim != indelim)
        std::cout << outdelim;
}

std::pair<vecsize_t, std::string> readcache()
{
    assertexists();
    std::ifstream fl(cachefl, std::ios::in | std::ios::binary);

    if (!fl)
        die("could not open cache file for reading");

    vecsize_t size;
    fl.read(reinterpret_cast<char*>(&size), sizeof(size));

    std::string buf;
    {
        std::ostringstream ss;
        ss << fl.rdbuf();
        buf = ss.str();
    }

    return std::move<std::pair<vecsize_t, std::string>>({ size, buf });
}

void writecache(std::vector<std::string> vec)
{
    std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!fl)
        die("could not open cache file for writing");

    vecsize_t size = vec.size();
    fl.write(reinterpret_cast<char*>(&size), sizeof(size));
    std::for_each(vec.begin(), vec.end(), [&fl](std::string& str) {
        fl.write(str.c_str(), str.length() + 1);
    });
}

vecsize_t readsize()
{
    assertexists();
    std::ifstream fl(cachefl, std::ios::in | std::ios::binary);

    if (!fl)
        die("could not open cache file for reading");

    vecsize_t size;
    fl.read(reinterpret_cast<char*>(&size), sizeof(size));

    return std::move(size);
}

vecsize_t parseindex(const std::string& indexstr)
{
    std::stringstream ss;
    ss << indexstr;

    vecsize_t index;
    ss >> index;

    if (ss.fail())
        die("index is not a valid integer");

    return std::move(index);
}

std::vector<std::string> readvec()
{
    const auto& [size, buf] = readcache();
    std::vector<std::string> vec;
    vec.reserve(size);
    strutil::split(vec, buf, indelim);
    return vec;
}

std::string buildcachefl(const std::string& vecname)
{
    std::string cachefl;
    {
        std::ostringstream ss;
        ss << prefix << '/' << givenexecname << '/' << proghash;
        ss << '/' << vecname;
        cachefl = ss.str();
    }
    return cachefl;
}

void vecout()
{
    const auto& [size, buf] = readcache();
    for (const auto&& view : buf | vecview | std::views::take(size))
        std::cout << view << outdelim;
}

void vecnew()
{
    std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!fl)
        die("could not open cache file for writing");

    vecsize_t size = 0;
    fl.write(reinterpret_cast<char*>(&size), sizeof(size));
}

void vecsize()
{
    std::cout << readsize();
    conditionaldelim();
}

void vecpushback(const std::string&& value)
{
    assertexists();
    std::fstream fl(cachefl, std::ios::in | std::ios::out | std::ios::binary);

    if (!fl)
        die("could not open cache file for reading and writing");

    vecsize_t size;
    fl.read(reinterpret_cast<char*>(&size), sizeof(size));

    if (size == std::numeric_limits<vecsize_t>::max())
        die("vector is at maximum capacity");
    size++;

    fl.seekg(0, std::ios::beg);
    fl.write(reinterpret_cast<char*>(&size), sizeof(size));
    fl.seekg(0, std::ios::end);
    fl.write(value.c_str(), value.length() + 1);
}

void vecpopback(const std::string&& countstr)
{
    auto count = parseindex(countstr);
    auto vec = readvec();

    if (count > vec.size())
        die("pop_back count cannot be greater than vector size");

    for ([[maybe_unused]] const auto&& _ : std::views::iota(0u, count)) {
        std::cout << vec.back();
        conditionaldelim();

        vec.pop_back();
    }

    writecache(vec);
}

void vecgetindex(const std::string&& indexstr)
{
    auto index = parseindex(indexstr);

    // TODO: reuse file stream
    if (auto size = readsize(); !size || index > size - 1)
        die("index is out of range");

    // TODO: reuse file stream
    const auto& [size, buf] = readcache();

    auto view = buf | vecview | std::views::drop(index);
    std::cout << view.front();
}

void vecsetindex(const std::string&& indexstr, const std::string&& value)
{
    auto index = parseindex(indexstr);

    // TODO: reuse file stream
    if (auto size = readsize(); !size || index > size - 1)
        die("index is out of range");

    // TODO: reuse file stream
    writecache(vecutil::setindex(readvec(), index, value));
}

void vecset(const std::string&& other)
{
    if (other == "NULL" || other == "nullptr") {
        std::filesystem::remove(cachefl);
    } else if (other == "") {
        die("OTHER_VEC_NAME cannot be empty");
    } else if (other == vecname) {
        die("OTHER_VEC_NAME cannot be the same as VEC_NAME");
    } else if (other.contains('/')) {
        die("OTHER_VEC_NAME cannot contain '/'");
    } else {
        std::string othercachefl = buildcachefl(other);
        assertexists(othercachefl);
        std::filesystem::copy_file(
            othercachefl, cachefl, std::filesystem::copy_options::overwrite_existing);
    }
}

void vecswap(const std::string&& other)
{
    assertexists();
    if (other == "" || other == "NULL" || other == "nullptr") {
        die("OTHER_VEC_NAME cannot be \"NULL\", \"nullptr\" or empty");
    } else if (other == vecname) {
        die("OTHER_VEC_NAME cannot be the same as VEC_NAME");
    } else if (other.contains('/')) {
        die("OTHER_VEC_NAME cannot contain '/'");
    } else {
        auto othercachefl = buildcachefl(other);
        assertexists(othercachefl);
        auto tmpfl = std::tmpnam(nullptr);
        std::filesystem::rename(cachefl, tmpfl);
        std::filesystem::rename(othercachefl, cachefl);
        std::filesystem::rename(tmpfl, othercachefl);
        std::filesystem::remove(tmpfl);
    }
}
