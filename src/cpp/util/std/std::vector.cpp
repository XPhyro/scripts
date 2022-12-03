// C++
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// C
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// C++ libraries
#include <lexical_cast.hpp>
#include <macros.hpp>
#include <nullable.hpp>
#include <strutil.hpp>
#include <sysutil.hpp>
#include <vecutil.hpp>

// C libraries
#include <dbutil.h>
#include <strutil.h>

// third-party
#include <pstream.h>

enum class cache { temporary, persistent };

typedef std::uint64_t vecsize_t;

std::set<std::string> acquiredlocks;
std::string cachefl;
int exit_code = EXIT_SUCCESS;

void lock_database(const std::string& hash);
inline void lock_database(const std::string&& hash);
void unlock_database(const std::string& hash);
inline void unlock_database(const std::string&& hash);
void unlock_all_databases();
[[noreturn]] void unknown_syntax();
[[noreturn]] void die(const std::string& err);
[[noreturn]] void terminate(int ec = EXIT_SUCCESS);
void handle_sig(int sig);
cache parse_args(int& argc, char**& argv);
bool exists(const std::string& path = cachefl);
void assertexists(const std::string& path = cachefl);
void conditional_delim();
void shell_escape(std::string& str);

namespace vec {
    vecsize_t parse_index(const std::string& indexstr);
    std::vector<std::string> parse();
    std::string build_cache_path(const std::string& vecname);
    std::pair<vecsize_t, std::string> read();
    void write(std::vector<std::string> vec);
    vecsize_t read_size();
    void get();
    void init();
    void ensure_init();
    void clear();
    void eval();
    void size();
    void front();
    void back();
    void insert(const std::string&& indexstr, const std::string&& value);
    void erase(const std::string&& indexstr);
    void pop_back(const std::string&& countstr);
    void push_back(int argc, char* argv[]);
    void emplace_back(int argc, char* argv[]);
    void get_index(const std::string&& indexstr);
    void set_index(const std::string&& indexstr, const std::string&& value);
    void set(const std::string&& other);
    void swap(const std::string&& other);
    void find(const std::optional<std::size_t> first_index,
              const std::optional<std::size_t> last_index,
              const std::string&& value);
} // namespace vec

const std::unordered_map<std::string, cache> caches = {
    { "t", cache::temporary },    { "tmp", cache::temporary },
    { "temp", cache::temporary }, { "temporary", cache::temporary },
    { "p", cache::persistent },   { "persistent", cache::persistent },
};
constexpr const char indelim = '\0';
constexpr const auto vecview = xph::str::splitview(indelim);
constexpr const lckdb_t lcktype = LCKDB_TEMP;
const std::string givenexecname = "vector";

std::string execname, proghash, vecname;
xph::nullable<char> outdelim(indelim);
const char* prefix;
bool opthash = false;

int main(int argc, char* argv[])
{
    if (!argc)
        execname = "NULL";
    else
        execname = argv[0];

    const auto cache = parse_args(argc, argv);

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

    if (opthash) {
        xph::str::hash_in_place(proghash);
        xph::str::hash_in_place(vecname);
    }

    switch (cache) {
        case cache::temporary:
            prefix = temphome();
            break;
        case cache::persistent:
            prefix = cachehome();
            break;
        default:
            die("unknown cache type");
    }

    xph::sys::signals({ SIGINT, SIGTERM, SIGQUIT, SIGHUP }, handle_sig);

    lock_database(givenexecname + '-' + proghash);
    lock_database(givenexecname + '-' + proghash + '-' + vecname);

    {
        std::string cachedir;
        {
            std::ostringstream ss;
            ss << prefix << '/' << givenexecname << '/' << proghash;
            cachedir = ss.str();

            if (argc == 1 && vecname == "=" &&
                (streq(argv[0], "NULL") || streq(argv[0], "nullptr"))) {
                std::filesystem::remove_all(cachedir);
                terminate(EXIT_SUCCESS);
            }

            ss << '/' << vecname;
            cachefl = ss.str();
        }

        std::filesystem::create_directories(cachedir);
    }

    if (vecname == "=")
        die("VECNAME cannot be \"=\"");

    if (argc) {
        STRING_SWITCH(argv[0])
        STRING_CASE("push_back")
        vec::push_back(argc - 1, argv + 1);
        STRING_BREAK
        STRING_CASE("emplace_back")
        if (argc < 2)
            die("no command given to emplace_back");
        vec::emplace_back(argc - 1, argv + 1);
        STRING_BREAK
        STRING_DEFAULT
        goto non_variadic;
        STRING_BREAK
    } else {
non_variadic:
        switch (argc) {
            case 0:
                vec::get();
                break;
            case 1: {
                STRING_SWITCH(argv[0])
                STRING_CASE("new")
                vec::init();
                STRING_BREAK
                STRING_CASE("ensure")
                vec::ensure_init();
                STRING_BREAK
                STRING_CASE("clear")
                vec::clear();
                STRING_BREAK
                STRING_CASE("eval")
                vec::eval();
                STRING_BREAK
                STRING_CASE("size")
                vec::size();
                STRING_BREAK
                STRING_CASE("front")
                vec::front();
                STRING_BREAK
                STRING_CASE("back")
                vec::back();
                STRING_BREAK
                STRING_CASE("pop_back")
                vec::pop_back("1");
                STRING_BREAK
                STRING_DEFAULT
                vec::get_index(argv[0]);
                STRING_BREAK
            } break;
            case 2: {
                STRING_SWITCH(argv[0])
                STRING_CASE("=")
                vec::set(argv[1]);
                STRING_BREAK
                STRING_CASE("pop_back")
                vec::pop_back(argv[1]);
                STRING_BREAK
                STRING_CASE("swap")
                vec::swap(argv[1]);
                STRING_BREAK
                STRING_CASE("erase")
                vec::erase(argv[1]);
                STRING_BREAK
                STRING_CASE("find")
                vec::find({}, {}, argv[1]);
                STRING_BREAK
                STRING_DEFAULT
                unknown_syntax();
                STRING_BREAK
            } break;
            case 3:
                if (streq(argv[1], "="))
                    vec::set_index(argv[0], argv[2]);
                else if (streq(argv[0], "insert"))
                    vec::insert(argv[1], argv[2]);
                else
                    unknown_syntax();
                break;
            case 4: {
                STRING_SWITCH(argv[0])
                STRING_CASE("find")
                vec::find(xph::lexical_cast<char*, std::size_t>(argv[1]),
                          xph::lexical_cast<char*, std::size_t>(argv[2]),
                          argv[3]);
                STRING_BREAK
                STRING_DEFAULT
                unknown_syntax();
                STRING_BREAK
            } break;
            default:
                unknown_syntax();
        }
    }

    unlock_all_databases();

    return exit_code;
}

void lock_database(const std::string& hash)
{
    if (!lckdb(hash.c_str(), lcktype))
        die("could not lock database");

    acquiredlocks.insert(hash);
}

inline void lock_database(const std::string&& hash)
{
    lock_database(hash);
}

void unlock_database(const std::string& hash)
{
    if (!ulckdb(hash.c_str(), lcktype))
        die("could not unlock database");

    acquiredlocks.erase(hash);
}

inline void unlock_database(const std::string&& hash)
{
    unlock_database(hash);
}

void unlock_all_databases()
{
    for (auto&& lock : acquiredlocks) {
        if (!ulckdb(lock.c_str(), lcktype))
            std::cerr << execname << ": could not unlock database " << lock << '\n';
    }
}

[[noreturn]] void unknown_syntax()
{
    die("unknown syntax");
}

[[noreturn]] void die(const std::string& err)
{
    std::cerr << execname << ": " << err << '\n';
    terminate(EXIT_FAILURE);
}

[[noreturn]] void terminate(int ec)
{
    unlock_all_databases();
    std::exit(ec);
}

void handle_sig([[maybe_unused]] int sig)
{
    terminate(EXIT_FAILURE);
}

cache parse_args(int& argc, char**& argv)
{
    cache cache = cache::temporary;
    int i;
    int optquiet = 0;

    while ((i = getopt(argc, argv, "c:Hhnqz0")) != -1) {
        switch (i) {
            case 'c':
                try {
                    cache = caches.at(xph::str::makelower(optarg));
                } catch (const std::out_of_range& e) {
                    std::cerr << execname << ": unknown cache type " << optarg << '\n';
                    terminate(EXIT_FAILURE);
                }
                break;
            case 'H':
                opthash = true;
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
                       "   2. ensure\n"
                       "      1. Initialise vector if not already initialised.\n"
                       "   3. clear\n"
                       "      1. Clear the contents of the vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   4. eval\n"
                       "      1. Print vector in a format `eval`able by a POSIX shell.\n"
                       "      2. Vector must have been initialised.\n"
                       "   5. size\n"
                       "      1. Get vector size.\n"
                       "      2. Vector must have been initialised.\n"
                       "   6. front\n"
                       "      1. Get front element of vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   7. back\n"
                       "      1. Get back element of vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "   8. insert [INDEX] [VALUE]\n"
                       "      1. Insert VALUE at INDEX.\n"
                       "      2. Vector must have been initialised.\n"
                       "   9. erase [INDEX]\n"
                       "      1. Erase value at INDEX.\n"
                       "      2. Vector must have been initialised.\n"
                       "  10. push_back [VALUE...]\n"
                       "      1. Append VALUEs to the end of the vector.\n"
                       "      2. Vector must have been initialised.\n"
                       "  11. emplace_back [COMMAND] [ARG...]\n"
                       "      1. Execute the given command with the given arguments, if any, and push_back its output after removing nulls (\\0).\n"
                       "      2. Vector must have been initialised.\n"
                       "  12. pop_back [COUNT]?\n"
                       "      1. Pop COUNT values from the end of the vector.\n"
                       "      2. If COUNT is not given, COUNT is 1.\n"
                       "      3. Vector must have been initialised.\n"
                       "  13. swap [OTHER_VEC_NAME]\n"
                       "      1. Swap VEC_NAME and OTHER_VEC_NAME.\n"
                       "      2. OTHER_VEC_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty, or contain '/'.\n"
                       "      3. Vectors must have been initialised.\n"
                       "  14. find [[FIRST_INDEX] [LAST_INDEX]]? [VALUE]\n"
                       "      1. Find VALUE in the vector.\n"
                       "      2. If FIRST_INDEX and LAST_INDEX are given, the search is limited within the range.\n"
                       "      3. If VALUE is found, its index is printed; otherwise the size of the vector is printed and a non-zero exit code is returned.\n"
                       "      4. Vector must have been initialised.\n"
                       "\n"
                       "PROG_HASH cannot be \"NULL\", \"nullptr\" or empty, or contain '/'.\n"
                       "VEC_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty, or contain '/'.\n"
                       "\n"
                       "Output delimiter is newline if stdout is a tty and null otherwise.\n"
                       "\n"
                       "Options\n"
                       "  -c TYPE   set cache type. must be one of {{t, tmp, temp, temporary}, {p, persistent}}. default is temporary.\n"
                       "  -H        hash PROG_HASH, VEC_NAME and OTHER_VEC_NAME when writing to cache. note that std::hash is used for hashing, so the resultant hash is not cryptographically secure, and the contents of the vectors are not encrypted.\n"
                       "  -h        display this help and exit\n"
                       "  -n        force output delimiter to be newline (\\n)\n"
                       "  -q        if given once, do not use stdout; if given twice or more, do not use stdout or stderr\n"
                       "  -z        force output delimiter to be null (\\0)\n"
                       "  -0        force output delimiter to be null (\\0)\n";
                terminate(EXIT_SUCCESS);
            case 'n':
                outdelim = '\n';
                break;
            case 'q':
                if (optquiet < 2) {
                    if (!optquiet)
                        std::cout.setstate(std::ios_base::failbit);
                    else
                        std::cerr.setstate(std::ios_base::failbit);
                    ++optquiet;
                }
                break;
            case 'z':
            case '0':
                outdelim = '\0';
                break;
            default:
                std::cerr << "Try '" << execname << " -h' for more information.\n";
                terminate(EXIT_FAILURE);
        }
    }

    argv += optind;
    argc -= optind;

    if (!outdelim && isatty(fileno(stdout)))
        outdelim = '\n';

    return cache;
}

bool exists(const std::string& path)
{
    return std::filesystem::exists(path);
}

void assertexists(const std::string& path)
{
    if (!exists(path))
        die("vector is not initialised");
}

void conditional_delim()
{
    if (outdelim != indelim)
        std::cout << outdelim;
}

void shell_escape(std::string& str)
{
    xph::str::replaceall(str, "'", "'\\''");
}

namespace vec {
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

        return { size, buf };
    }

    void write(std::vector<std::string> vec)
    {
        std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

        if (!fl)
            die("could not open cache file for writing");

        const vecsize_t size = vec.size();
        fl.write(reinterpret_cast<const char*>(&size), sizeof(size));
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

        return size;
    }

    vecsize_t parse_index(const std::string& indexstr)
    {
        std::stringstream ss;
        ss << indexstr;

        vecsize_t index;
        ss >> index;

        if (ss.fail())
            die("index is not a valid integer");

        return index;
    }

    std::vector<std::string> parse()
    {
        const auto& [size, buf] = read();
        std::vector<std::string> vec;
        vec.reserve(size);
        xph::str::split(vec, buf, indelim);
        return vec;
    }

    std::string build_cache_path(const std::string& vecname)
    {
        std::string cachefl;
        {
            std::ostringstream ss;
            ss << prefix << '/' << givenexecname << '/' << proghash;
            ss << '/' << (!opthash ? vecname : xph::str::hash(vecname));
            cachefl = ss.str();
        }
        return cachefl;
    }

    void get()
    {
        for (const auto& [size, buf] = read();
             const auto&& view : buf | vecview | std::views::take(size))
            std::cout << view << outdelim;
    }

    void init()
    {
        std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

        if (!fl)
            die("could not open cache file for writing");

        const vecsize_t size = 0;
        fl.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    void ensure_init()
    {
        if (!exists(cachefl))
            init();
    }

    void clear()
    {
        assertexists();
        init();
    }

    void size()
    {
        std::cout << read_size();
        conditional_delim();
    }

    void push_back(int argc, char* argv[])
    {
        assertexists();
        std::fstream fl(cachefl, std::ios::in | std::ios::out | std::ios::binary);

        if (!fl)
            die("could not open cache file for reading and writing");

        vecsize_t size;
        fl.read(reinterpret_cast<char*>(&size), sizeof(size));

        if (size == std::numeric_limits<vecsize_t>::max())
            die("vector is at maximum capacity");

        for (const auto& line : std::views::counted(argv, argc)) {
            ++size;

            fl.seekg(0, std::ios::beg);
            fl.write(reinterpret_cast<char*>(&size), sizeof(size));

            fl.seekg(0, std::ios::end);
            fl.write(line, strlen(line) + 1);

            if (size >= std::numeric_limits<vecsize_t>::max())
                die("vector is at maximum capacity");
        }
    }

    void emplace_back(int argc, char* argv[])
    {
        assertexists();
        std::fstream fl(cachefl, std::ios::in | std::ios::out | std::ios::binary);

        if (!fl)
            die("could not open cache file for reading and writing");

        vecsize_t size;
        fl.read(reinterpret_cast<char*>(&size), sizeof(size));

        if (size >= std::numeric_limits<vecsize_t>::max())
            die("vector is at maximum capacity");

        // TODO: replace with the following with C++23
        // auto exec_argv = std::ranges::to<std::vector>(std::views::counted(argv, argc));
        std::vector<std::string> exec_argv;
        exec_argv.reserve(argc);
        std::ranges::copy(std::views::counted(argv, argc), std::back_inserter(exec_argv));

        redi::ipstream proc(exec_argv, redi::pstreams::pstdout);
        std::string line;
        while (std::getline(proc.out(), line, '\0')) { // TODO: make delimiter configurable
            ++size;

            fl.seekg(0, std::ios::beg);
            fl.write(reinterpret_cast<char*>(&size), sizeof(size));

            fl.seekg(0, std::ios::end);
            fl.write(line.c_str(), line.length() + 1);

            if (size >= std::numeric_limits<vecsize_t>::max())
                die("vector is at maximum capacity");
        }

        if (proc.eof() && proc.fail())
            proc.clear();
    }

    void pop_back(const std::string&& countstr)
    {
        const auto count = parse_index(countstr);
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
        const auto index = parse_index(indexstr);
        const auto& [size, buf] = read();

        if (!size || index > size - 1)
            die("index is out of range");

        auto view = buf | vecview | std::views::drop(index);
        std::cout << view.front();
    }

    void set_index(const std::string&& indexstr, const std::string&& value)
    {
        const auto index = parse_index(indexstr);

        // TODO: reuse file stream
        if (const auto size = read_size(); !size || index > size - 1)
            die("index is out of range");

        // TODO: reuse file stream
        write(xph::vec::setindex(parse(), index, value));
    }

    void set(const std::string&& other)
    {
        if (other == "NULL" || other == "nullptr") {
            std::filesystem::remove(cachefl);
        } else if (other == "" || other == "=") {
            die("OTHER_VEC_NAME cannot be \"=\" or empty");
        } else if (other == vecname) {
            die("OTHER_VEC_NAME cannot be the same as VEC_NAME");
        } else if (other.contains('/')) {
            die("OTHER_VEC_NAME cannot contain '/'");
        } else {
            const auto otherlckhash = execname + '-' + proghash + '-' + other;
            lock_database(otherlckhash);
            const auto othercachefl = build_cache_path(other);
            assertexists(othercachefl);
            std::filesystem::copy_file(
                othercachefl, cachefl, std::filesystem::copy_options::overwrite_existing);
            unlock_database(otherlckhash);
        }
    }

    void swap(const std::string&& other)
    {
        assertexists();
        if (other == "" || other == "=" || other == "NULL" || other == "nullptr") {
            die("OTHER_VEC_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty");
        } else if (other == vecname) {
            die("OTHER_VEC_NAME cannot be the same as VEC_NAME");
        } else if (other.contains('/')) {
            die("OTHER_VEC_NAME cannot contain '/'");
        } else {
            const auto otherlckhash = execname + '-' + proghash + '-' + other;
            lock_database(otherlckhash);
            const auto othercachefl = build_cache_path(other);
            assertexists(othercachefl);
            xph::sys::swapfile(cachefl, othercachefl);
            unlock_database(otherlckhash);
        }
    }

    void find(const std::optional<std::size_t> first_index,
              const std::optional<std::size_t> last_index,
              const std::string&& value)
    {
        const auto vec = parse();
        const auto begin = vec.begin();
        auto idx = static_cast<std::size_t>(
            std::find(begin + (first_index ? first_index.value() : 0),
                      begin + (last_index ? last_index.value() : vec.size()),
                      value) -
            begin);
        std::cout << idx;
        if (idx == vec.size())
            exit_code = EXIT_FAILURE;
    }

    void front()
    {
        const auto vec = parse();
        if (vec.empty())
            die("cannot get front element of empty vector");
        std::cout << vec.front();
        conditional_delim();
    }

    void back()
    {
        const auto vec = parse();
        if (vec.empty())
            die("cannot get back element of empty vector");
        std::cout << vec.back();
        conditional_delim();
    }

    void insert(const std::string&& indexstr, const std::string&& value)
    {
        const auto index = parse_index(indexstr);
        auto vec = parse();

        if (index > vec.size())
            die("index is out of range");

        vec.insert(vec.begin() + index, value);

        write(vec);
    }

    void erase(const std::string&& indexstr)
    {
        const auto index = parse_index(indexstr);
        auto vec = parse();

        if (vec.empty() || index > vec.size() - 1)
            die("index is out of range");

        vec.erase(vec.begin() + index);

        write(vec);
    }

    void eval()
    {
        std::cout << "set --";
        for (auto vec = parse(); auto&& str : vec) {
            shell_escape(str);
            std::cout << " '" << str << '\'';
        }
    }
} // namespace vec
