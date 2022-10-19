// C++
#include <algorithm>
#include <cstddef>
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

typedef std::size_t mapsize_t;

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
void assertexists(const std::string path = cachefl);
void conditional_delim();
void shell_escape(std::string& str);

namespace map {
    std::unordered_map<std::string, std::string> parse();
    std::string build_cache_path(const std::string& vecname);
    std::pair<mapsize_t, std::string> read();
    void write(std::unordered_map<std::string, std::string> vec);
    mapsize_t read_size();
    void get();
    void init();
    void clear();
    void size();
    void insert(const std::string&& key, const std::string&& value);
    void erase(const std::string&& key);
    void emplace(int argc, char* argv[]);
    void get_key(const std::string&& key);
    void set_key(const std::string&& key, const std::string&& value);
    void set(const std::string&& other);
    void swap(const std::string&& other);
    void find(const std::string&& value);
} // namespace map

const std::unordered_map<std::string, cache> caches = {
    { "t", cache::temporary },    { "tmp", cache::temporary },
    { "temp", cache::temporary }, { "temporary", cache::temporary },
    { "p", cache::persistent },   { "persistent", cache::persistent },
};
constexpr const char indelim = '\0';
constexpr const auto vecview = xph::str::splitview(indelim);
constexpr const lckdb_t lcktype = LCKDB_TEMP;
const std::string givenexecname = "unordered_map";

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
        die("MAP_NAME cannot be \"NULL\", \"nullptr\" or empty");
    if (vecname.contains('/'))
        die("MAP_NAME cannot contain '/'");

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
        die("MAPNAME cannot be \"=\"");

    if (argc) {
        STRING_SWITCH(argv[0])
        STRING_CASE("emplace")
        if (argc < 3)
            unknown_syntax();
        map::emplace(argc - 1, argv + 1);
        STRING_BREAK
        STRING_DEFAULT
        goto non_variadic;
        STRING_BREAK
    } else {
non_variadic:
        switch (argc) {
            case 0:
                map::get();
                break;
            case 1: {
                STRING_SWITCH(argv[0])
                STRING_CASE("new")
                map::init();
                STRING_BREAK
                STRING_CASE("clear")
                map::clear();
                STRING_BREAK
                STRING_CASE("size")
                map::size();
                STRING_BREAK
                STRING_DEFAULT
                map::get_key(argv[0]);
                STRING_BREAK
            } break;
            case 2: {
                STRING_SWITCH(argv[0])
                STRING_CASE("=")
                map::set(argv[1]);
                STRING_BREAK
                STRING_CASE("swap")
                map::swap(argv[1]);
                STRING_BREAK
                STRING_CASE("erase")
                map::erase(argv[1]);
                STRING_BREAK
                STRING_CASE("find")
                map::find(argv[1]);
                STRING_BREAK
                STRING_DEFAULT
                unknown_syntax();
                STRING_BREAK
            } break;
            case 3:
                if (streq(argv[1], "="))
                    map::set_key(argv[0], argv[2]);
                else if (streq(argv[0], "insert"))
                    map::insert(argv[1], argv[2]);
                else
                    unknown_syntax();
                break;
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
                       "Handle maps in a strictly POSIX shell.\n"
                       "\n"
                       "SYNTAX may be one of:\n"
                       "1. = {NULL, nullptr}\n"
                       "   1. Uninitialise all maps belonging to PROG_HASH.\n"
                       "2. [MAP_NAME]\n"
                       "   1. Get map.\n"
                       "   2. Map must have been initialised.\n"
                       "3. [MAP_NAME] [KEY]\n"
                       "   1. Get value at KEY.\n"
                       "   2. Map must have been initialised.\n"
                       "4. [MAP_NAME] = [OTHER_MAP_NAME]\n"
                       "   1. Copy OTHER_MAP_NAME to MAP_NAME.\n"
                       "   2. If OTHER_MAP_NAME is NULL or nullptr, uninitialise map. Otherwise, OTHER_MAP_NAME must have been initialised.\n"
                       "5. [MAP_NAME] [KEY] = [VALUE]\n"
                       "   1. Set value at KEY to VALUE.\n"
                       "   2. KEY must be in range [0, size).\n"
                       "   3. Map must have been initialised.\n"
                       "6. [MAP_NAME] [FUNC]\n"
                       "   1. new\n"
                       "      1. Initialise map.\n"
                       "      2. If already initialised, map is reinitialised.\n"
                       "   2. clear\n"
                       "      1. Clear the contents of the map.\n"
                       "      2. Map must have been initialised.\n"
                       "   3. size\n"
                       "      1. Get map size.\n"
                       "      2. Map must have been initialised.\n"
                       "   4. insert [KEY] [VALUE]\n"
                       "      1. Insert VALUE at KEY.\n"
                       "      2. Map must have been initialised.\n"
                       "   5. erase [KEY]\n"
                       "      1. Erase value at KEY.\n"
                       "      2. Map must have been initialised.\n"
                       "   6. emplace [KEY] [COMMAND] [ARG...]\n"
                       "      1. Execute the given command with the given arguments, if any, and insert its output after removing nulls (\\0).\n"
                       "      2. Map must have been initialised.\n"
                       "   7. swap [OTHER_MAP_NAME]\n"
                       "      1. Swap MAP_NAME and OTHER_MAP_NAME.\n"
                       "      2. OTHER_MAP_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty, or contain '/'.\n"
                       "      3. Maps must have been initialised.\n"
                       "   8. find [VALUE]\n"
                       "      1. Find VALUE in the map.\n"
                       "      2. If VALUE is found, its key is printed; otherwise \"NULL\" is printed and a non-zero exit code is returned.\n"
                       "      3. Map must have been initialised.\n"
                       "\n"
                       "PROG_HASH cannot be \"NULL\", \"nullptr\" or empty, or contain '/'.\n"
                       "MAP_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty, or contain '/'.\n"
                       "KEY cannot be \"NULL\", \"nullptr\" or empty.\n"
                       "\n"
                       "Output delimiter is newline if stdout is a tty and null otherwise.\n"
                       "\n"
                       "Options\n"
                       "  -c TYPE   set cache type. must be one of {{t, tmp, temp, temporary}, {p, persistent}}. default is temporary.\n"
                       "  -H        hash PROG_HASH, MAP_NAME and OTHER_MAP_NAME when writing to cache. note that std::hash is used for hashing, so the resultant hash is not cryptographically secure, and the contents of the maps are not encrypted.\n"
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

void assertexists(const std::string path)
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
    xph::str::replaceall(str, "'", "'\\''");
}

namespace map {
    std::pair<mapsize_t, std::string> read()
    {
        assertexists();
        std::ifstream fl(cachefl, std::ios::in | std::ios::binary);

        if (!fl)
            die("could not open cache file for reading");

        mapsize_t size;
        fl.read(reinterpret_cast<char*>(&size), sizeof(size));

        std::string buf;
        {
            std::ostringstream ss;
            ss << fl.rdbuf();
            buf = ss.str();
        }

        return { size, buf };
    }

    void write(std::unordered_map<std::string, std::string> map)
    {
        std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

        if (!fl)
            die("could not open cache file for writing");

        const mapsize_t size = map.size();
        fl.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, val] : map) {
            fl.write(key.c_str(), key.length() + 1);
            fl.write(val.c_str(), val.length() + 1);
        }
    }

    mapsize_t read_size()
    {
        assertexists();
        std::ifstream fl(cachefl, std::ios::in | std::ios::binary);

        if (!fl)
            die("could not open cache file for reading");

        mapsize_t size;
        fl.read(reinterpret_cast<char*>(&size), sizeof(size));

        return std::move(size);
    }

    std::unordered_map<std::string, std::string> parse()
    {
        const auto& [size, buf] = read();

        std::unordered_map<std::string, std::string> map;
        if (!size)
            return map;

        std::vector<std::string> vec;
        vec.reserve(size);
        xph::str::split(vec, buf, indelim);

        for (auto it = vec.begin(); it < vec.end() - 1; it += 2)
            map.insert({ it[0], it[1] });

        return map;
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
             const auto&& view : buf | vecview | std::views::take(size * 2))
            std::cout << view << outdelim;
    }

    void init()
    {
        std::ofstream fl(cachefl, std::ios::out | std::ios::trunc | std::ios::binary);

        if (!fl)
            die("could not open cache file for writing");

        const mapsize_t size = 0;
        fl.write(reinterpret_cast<const char*>(&size), sizeof(size));
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

    void emplace(int argc, char* argv[])
    {
        assertexists();
        std::fstream fl(cachefl, std::ios::in | std::ios::out | std::ios::binary);

        if (!fl)
            die("could not open cache file for reading and writing");

        mapsize_t size;
        fl.read(reinterpret_cast<char*>(&size), sizeof(size));

        if (size >= std::numeric_limits<mapsize_t>::max())
            die("vector is at maximum capacity");

        auto map = parse();
        const std::string key = argv[0];
        if (map.contains(key))
            die("map already contains given key");

        fl.seekg(0, std::ios::end);
        fl.write(key.c_str(), key.length() + 1);

        // TODO: replace with the following with C++23
        // auto exec_argv = std::ranges::to<std::vector>(std::views::counted(argv, argc));
        std::vector<std::string> exec_argv;
        exec_argv.reserve(argc);
        for (const auto& arg : std::views::counted(argv + 1, argc - 1))
            exec_argv.emplace_back(arg);

        redi::ipstream proc(exec_argv, redi::pstreams::pstdout);
        std::string line;
        std::string value = "";
        while (std::getline(proc.out(), line, '\0')) // TODO: make delimiter configurable
            value += line;
        map.insert({ key, value });
        write(map);

        if (proc.eof() && proc.fail())
            proc.clear();
    }

    void get_key(const std::string&& key)
    {
        auto map = parse();
        if (!map.contains(key))
            die("map does not contain key");
        std::cout << map[key];
    }

    void set_key(const std::string&& key, const std::string&& value)
    {
        auto map = parse();

        if (map.contains(key))
            map[key] = value;
        else
            map.insert({ key, value });

        write(map);
    }

    void set(const std::string&& other)
    {
        if (other == "NULL" || other == "nullptr") {
            std::filesystem::remove(cachefl);
        } else if (other == "" || other == "=") {
            die("OTHER_MAP_NAME cannot be \"=\" or empty");
        } else if (other == vecname) {
            die("OTHER_MAP_NAME cannot be the same as MAP_NAME");
        } else if (other.contains('/')) {
            die("OTHER_MAP_NAME cannot contain '/'");
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
            die("OTHER_MAP_NAME cannot be \"NULL\", \"nullptr\", \"=\" or empty");
        } else if (other == vecname) {
            die("OTHER_MAP_NAME cannot be the same as MAP_NAME");
        } else if (other.contains('/')) {
            die("OTHER_MAP_NAME cannot contain '/'");
        } else {
            const auto otherlckhash = execname + '-' + proghash + '-' + other;
            lock_database(otherlckhash);
            const auto othercachefl = build_cache_path(other);
            assertexists(othercachefl);
            xph::sys::swapfile(cachefl, othercachefl);
            unlock_database(otherlckhash);
        }
    }

    void find(const std::string&& value)
    {
        const auto map = parse();

        for (const auto& [key, val] : map) {
            if (val == value) {
                std::cout << key;
                return;
            }
        }

        exit_code = EXIT_FAILURE;
    }

    void insert(const std::string&& key, const std::string&& value)
    {
        auto map = parse();

        if (map.contains(key))
            die("map already contains key");

        map.insert({ key, value });

        write(map);
    }

    void erase(const std::string&& key)
    {
        auto map = parse();

        if (!map.contains(key))
            die("map does not contain key");

        map.erase(key);

        write(map);
    }
} // namespace map
