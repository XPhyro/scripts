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
#include <sysutil.hpp>
#include <vecutil.hpp>

// C libraries
#include <strutil.h>

// third party
#include <hedley.h>

DEFINE_ENUM(cache, temporary COMMA persistent COMMA);

typedef std::size_t vecsize_t;

HEDLEY_NO_RETURN void die(const std::string& err);
cache parse_args(int* argc, char** argv[]);
void conditional_delim();
void shell_escape(std::string& str);
namespace vec
{
vecsize_t parse_index(const std::string& indexstr);
std::vector<std::string> parse();
std::string build_cache_file(const std::string& vecname);
std::pair<vecsize_t, std::string> read();
void write(std::vector<std::string> vec);
vecsize_t read_size();
void get();
void init();
void eval();
void size();
void front();
void back();
void insert(const std::string&& indexstr, const std::string&& value);
void erase(const std::string&& indexstr);
void pop_back(const std::string&& countstr);
void push_back(const std::string&& value);
void get_index(const std::string&& indexstr);
void set_index(const std::string&& indexstr, const std::string&& value);
void set(const std::string&& other);
void swap(const std::string&& other);
} // namespace vec

const std::unordered_map<std::string, cache> caches = {
    { "t", cache::temporary },    { "tmp", cache::temporary },
    { "temp", cache::temporary }, { "temporary", cache::temporary },
    { "p", cache::persistent },   { "persistent", cache::persistent },
};
const char indelim = '\0';
const auto constexpr vecview = strutil::splitview(indelim);
const std::string givenexecname = "vector";

std::string execname, proghash, vecname, cachefl;
char outdelim = indelim;
const char* prefix;

int main(int argc, char* argv[])
{
    if (!argc)
        execname = "NULL";
    else
        execname = argv[0];

    const auto cache = parse_args(&argc, &argv);

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
            die("unknown cache type");
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
            vec::push_back(val);
    } else {
        switch (argc) {
            case 0:
                vec::get();
                break;
            case 1:
                if (streq(argv[0], "new"))
                    vec::init();
                else if (streq(argv[0], "eval"))
                    vec::eval();
                else if (streq(argv[0], "size"))
                    vec::size();
                else if (streq(argv[0], "front"))
                    vec::front();
                else if (streq(argv[0], "back"))
                    vec::back();
                else if (streq(argv[0], "pop_back"))
                    vec::pop_back("1");
                else
                    vec::get_index(argv[0]);
                break;
            case 2:
                if (streq(argv[0], "="))
                    vec::set(argv[1]);
                else if (streq(argv[0], "pop_back"))
                    vec::pop_back(argv[1]);
                else if (streq(argv[0], "swap"))
                    vec::swap(argv[1]);
                else if (streq(argv[0], "erase"))
                    vec::erase(argv[1]);
                break;
            case 3:
                if (streq(argv[1], "="))
                    vec::set_index(argv[0], argv[2]);
                else if (streq(argv[0], "insert"))
                    vec::insert(argv[1], argv[2]);
                break;
            default:
                die("unknown syntax");
        }
    }

    return EXIT_SUCCESS;
}

HEDLEY_NO_RETURN void die(const std::string& err)
{
    std::cerr << execname << ": " << err << '\n';
    std::exit(EXIT_FAILURE);
}

cache parse_args(int* argc, char** argv[])
{
    cache cache = cache::temporary;
    int i;
    bool optoutdelim = false;
    int optquiet = 0;

    while ((i = getopt(*argc, *argv, "c:hnqz0")) != -1) {
        switch (i) {
            case 'c':
                try {
                    cache = caches.at(strutil::makelower(optarg));
                } catch (const std::out_of_range& e) {
                    std::cerr << execname << ": unknown cache type " << optarg << '\n';
                    std::exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                std::cout
                    << "Usage: " << execname
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
                       "   2. eval\n"
                       "      1. Print vector in a format `eval`able by a POSIX shell.\n"
                       "      2. Vector must have been initialised.\n"
                       "   3. size\n"
                       "      1. Get vector size.\n"
                       "      2. Vector must have been initialised.\n"
                       "   4. front\n"
                       "      1. Get front element of vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   5. back\n"
                       "      1. Get back element of vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   6. insert [INDEX] [VALUE]\n"
                       "      1. Insert VALUE at INDEX.\n"
                       "      2. Vector must have been initialised.\n"
                       "   7. erase [INDEX]\n"
                       "      1. Erase value at INDEX.\n"
                       "      2. Vector must have been initialised.\n"
                       "   8. push_back [VALUE...]\n"
                       "      1. Append VALUEs to the end of the vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   9. pop_back [COUNT]?\n"
                       "      1. Pop COUNT values from the end of the vector.\n"
                       "      2. If COUNT is not given, COUNT is 1.\n"
                       "      3. Vector must have been initialised.\n"
                       "  10. swap [OTHER_VEC_NAME]\n"
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
                       "  -q        if given once, do not use stdout; if given twice or more, do not use stdout or stderr\n"
                       "  -z        force output delimiter to be null (\\0)\n"
                       "  -0        force output delimiter to be null (\\0)\n";
                std::exit(EXIT_SUCCESS);
            case 'n':
                optoutdelim = true;
                outdelim = '\n';
                break;
            case 'q':
                if (optquiet < 2) {
                    if (!optquiet)
                        std::cout.setstate(std::ios_base::failbit);
                    else
                        std::cerr.setstate(std::ios_base::failbit);
                    optquiet++;
                }
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

void conditional_delim()
{
    if (outdelim != indelim)
        std::cout << outdelim;
}

void shell_escape(std::string& str)
{
    strutil::replaceall(str, "'", "'\\''");
}

namespace vec
{
std::pair<vecsize_t, std::string> read()
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

void write(std::vector<std::string> vec)
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

vecsize_t read_size()
{
    assertexists();
    std::ifstream fl(cachefl, std::ios::in | std::ios::binary);

    if (!fl)
        die("could not open cache file for reading");

    vecsize_t size;
    fl.read(reinterpret_cast<char*>(&size), sizeof(size));

    return std::move(size);
}

vecsize_t parse_index(const std::string& indexstr)
{
    std::stringstream ss;
    ss << indexstr;

    vecsize_t index;
    ss >> index;

    if (ss.fail())
        die("index is not a valid integer");

    return std::move(index);
}

std::vector<std::string> parse()
{
    const auto& [size, buf] = read();
    std::vector<std::string> vec;
    vec.reserve(size);
    strutil::split(vec, buf, indelim);
    return vec;
}

std::string build_cache_file(const std::string& vecname)
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

void get()
{
    const auto& [size, buf] = read();
    for (const auto&& view : buf | vecview | std::views::take(size))
        std::cout << view << outdelim;
}

void init()
{
    std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!fl)
        die("could not open cache file for writing");

    vecsize_t size = 0;
    fl.write(reinterpret_cast<char*>(&size), sizeof(size));
}

void size()
{
    std::cout << read_size();
    conditional_delim();
}

void push_back(const std::string&& value)
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

void pop_back(const std::string&& countstr)
{
    auto count = parse_index(countstr);
    auto vec = parse();

    if (count > vec.size())
        die("pop_back count cannot be greater than vector size");

    for ([[maybe_unused]] const auto&& _ : std::views::iota(0u, count)) {
        std::cout << vec.back();
        conditional_delim();

        vec.pop_back();
    }

    write(vec);
}

void get_index(const std::string&& indexstr)
{
    auto index = parse_index(indexstr);
    const auto& [size, buf] = read();

    if (!size || index > size - 1)
        die("index is out of range");

    auto view = buf | vecview | std::views::drop(index);
    std::cout << view.front();
}

void set_index(const std::string&& indexstr, const std::string&& value)
{
    auto index = parse_index(indexstr);

    // TODO: reuse file stream
    if (auto size = read_size(); !size || index > size - 1)
        die("index is out of range");

    // TODO: reuse file stream
    write(vecutil::setindex(parse(), index, value));
}

void set(const std::string&& other)
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
        std::string othercachefl = build_cache_file(other);
        assertexists(othercachefl);
        std::filesystem::copy_file(
            othercachefl, cachefl, std::filesystem::copy_options::overwrite_existing);
    }
}

void swap(const std::string&& other)
{
    assertexists();
    if (other == "" || other == "NULL" || other == "nullptr") {
        die("OTHER_VEC_NAME cannot be \"NULL\", \"nullptr\" or empty");
    } else if (other == vecname) {
        die("OTHER_VEC_NAME cannot be the same as VEC_NAME");
    } else if (other.contains('/')) {
        die("OTHER_VEC_NAME cannot contain '/'");
    } else {
        auto othercachefl = build_cache_file(other);
        assertexists(othercachefl);
        sysutil::swapfile(cachefl, othercachefl);
    }
}

void front()
{
    auto vec = parse();
    if (vec.empty())
        die("cannot get front element of empty vector");
    std::cout << vec.front();
    conditional_delim();
}

void back()
{
    auto vec = parse();
    if (vec.empty())
        die("cannot get back element of empty vector");
    std::cout << vec.back();
    conditional_delim();
}

void insert(const std::string&& indexstr, const std::string&& value)
{
    auto index = parse_index(indexstr);
    auto vec = parse();

    if (index > vec.size())
        die("index is out of range");

    vec.insert(vec.begin() + index, value);

    write(vec);
}

void erase(const std::string&& indexstr)
{
    auto index = parse_index(indexstr);
    auto vec = parse();

    if (vec.empty() || index > vec.size() - 1)
        die("index is out of range");

    vec.erase(vec.begin() + index);

    write(vec);
}

void eval()
{
    auto vec = parse();
    std::cout << "set --";
    for (auto&& str : vec) {
        shell_escape(str);
        std::cout << " '" << str << '\'';
    }
}
} // namespace vec
