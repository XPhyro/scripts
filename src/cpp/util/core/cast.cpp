// C++
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
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
#include <die.hpp>
#include <exec_info.hpp>
#include <lexical_cast.hpp>
#include <strutil.hpp>
#include <tuple_hash.hpp>

static_assert(PIPE_BUF % 8 == 0);
static_assert(sizeof(char) == 1 && sizeof(unsigned char) == 1);
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);
static_assert(std::endian::native == std::endian::little);

enum class type {
    bitset,
    character,
    float32,
    float64,
    float128,
    hex8,
    hex16,
    hex32,
    hex64,
    hexfloat32,
    hexfloat64,
    hexfloat128,
    int8,
    int16,
    int32,
    int64,
    intx,
    oct8,
    oct16,
    oct32,
    oct64,
    uint8,
    uint16,
    uint32,
    uint64,
    uintx,
};

[[noreturn]] void help();
[[noreturn]] void invalid_args(const std::string& err);
void assert_argc(int n, int argc);
void cast_bitset_to_character(int argc, char** argv);
void cast_character_to_bitset(int argc, char** argv);
template <typename T, auto converter, size_t size_override>
void cast_character_to_primitive(int argc, [[maybe_unused]] char** argv);
template <typename T, auto converter>
void cast_character_to_primitive(int argc, char** argv);
template <typename T, auto converter>
void cast_character_to_xintx(int argc, char** argv);

const std::unordered_map<std::string, type> types = {
    { "bit", type::bitset },
    { "char", type::character },
    { "float32", type::float32 },
    { "float64", type::float64 },
    { "float128", type::float128 },
    { "hex8", type::hex8 },
    { "hex16", type::hex16 },
    { "hex32", type::hex32 },
    { "hex64", type::hex64 },
    { "hexfloat32", type::hexfloat32 },
    { "hexfloat64", type::hexfloat64 },
    { "hexfloat128", type::hexfloat128 },
    { "int8", type::int8 },
    { "int16", type::int16 },
    { "int32", type::int32 },
    { "int64", type::int64 },
    { "intx", type::intx },
    { "oct8", type::oct8 },
    { "oct16", type::oct16 },
    { "oct32", type::oct32 },
    { "oct64", type::oct64 },
    { "uint8", type::uint8 },
    { "uint16", type::uint16 },
    { "uint32", type::uint32 },
    { "uint64", type::uint64 },
    { "uintx", type::uintx },
};

const std::unordered_map<std::tuple<type, type>, std::function<void(int, char**)>> funcs = {
    { { type::bitset, type::character }, cast_bitset_to_character },
    { { type::character, type::bitset }, cast_character_to_bitset },
    { { type::character, type::float32 }, cast_character_to_primitive<float, std::dec> },
    { { type::character, type::float64 }, cast_character_to_primitive<double, std::dec> },
    { { type::character, type::float128 }, cast_character_to_primitive<long double, std::dec> },
    { { type::character, type::hex8 },
      cast_character_to_primitive<int16_t, std::hex, sizeof(int8_t)> },
    { { type::character, type::hex16 }, cast_character_to_primitive<int16_t, std::hex> },
    { { type::character, type::hex32 }, cast_character_to_primitive<int32_t, std::hex> },
    { { type::character, type::hex64 }, cast_character_to_primitive<int64_t, std::hex> },
    { { type::character, type::hexfloat32 }, cast_character_to_primitive<float, std::hexfloat> },
    { { type::character, type::hexfloat64 }, cast_character_to_primitive<double, std::hexfloat> },
    { { type::character, type::hexfloat128 },
      cast_character_to_primitive<long double, std::hexfloat> },
    { { type::character, type::int8 },
      cast_character_to_primitive<int16_t, std::dec, sizeof(uint8_t)> },
    { { type::character, type::int16 }, cast_character_to_primitive<int16_t, std::dec> },
    { { type::character, type::int32 }, cast_character_to_primitive<int32_t, std::dec> },
    { { type::character, type::int64 }, cast_character_to_primitive<int64_t, std::dec> },
    { { type::character, type::intx }, cast_character_to_xintx<intmax_t, std::dec> },
    { { type::character, type::oct8 },
      cast_character_to_primitive<int16_t, std::oct, sizeof(int8_t)> },
    { { type::character, type::oct16 }, cast_character_to_primitive<int16_t, std::oct> },
    { { type::character, type::oct32 }, cast_character_to_primitive<int32_t, std::oct> },
    { { type::character, type::oct64 }, cast_character_to_primitive<int64_t, std::oct> },
    { { type::character, type::uint8 },
      cast_character_to_primitive<uint16_t, std::dec, sizeof(uint8_t)> },
    { { type::character, type::uint16 }, cast_character_to_primitive<uint16_t, std::dec> },
    { { type::character, type::uint32 }, cast_character_to_primitive<uint32_t, std::dec> },
    { { type::character, type::uint64 }, cast_character_to_primitive<uint64_t, std::dec> },
    { { type::character, type::uintx }, cast_character_to_xintx<uintmax_t, std::dec> },
};

DEFINE_EXEC_INFO();

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    for (int i; (i = getopt(argc, argv, "h")) != -1;) {
        switch (i) {
            case 'h':
                help();
            default:
                invalid_args(xph::consts::str::empty);
        }
    }

    argv += optind;
    argc -= optind;

    if (argc < 2)
        invalid_args(xph::consts::str::empty);

    using func_t = typename std::decay<decltype((*funcs.begin()).second)>::type;
    func_t func;
    try {
        auto fromtype = types.at(xph::str::makelower(std::string(argv[0])));
        auto totype = types.at(xph::str::makelower(std::string(argv[1])));
        func = funcs.at({ fromtype, totype });
    } catch (const std::out_of_range& e) {
        invalid_args("invalid type or type pair given");
    }

    argv += 2;
    argc -= 2;

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    func(argc, argv);
}

[[noreturn]] void help()
{
    std::cout << "Usage: " << xph::exec_name
              << " [OPTION...] [FROM_TYPE] [TO_TYPE] [CAST_ARGS...]\n"
                 "Cast types to types.\n"
                 "\n"
                 "Valid types are: ";

    std::vector<std::string> typenames;
    std::unordered_map<type, std::string> reverse_types;
    typenames.reserve(types.size());
    reverse_types.reserve(types.size());
    for (const auto& [name, type] : types) {
        typenames.push_back(name);
        assert(!reverse_types.contains(type));
        reverse_types.insert({ type, name });
    }
    std::sort(typenames.begin(), typenames.end());

    for (auto it = typenames.begin(); it < typenames.end() - 1; ++it)
        std::cout << *it << ", ";
    std::cout << typenames.back()
              << ".\n"
                 "\n"
                 "Valid casts are:\n";

    for (const auto& [pair, _] : funcs) {
        auto from = reverse_types[std::get<0>(pair)];
        auto to = reverse_types[std::get<1>(pair)];
        std::cout << "  " << from << " -> " << to << '\n';
    }

    const char* endianness = std::endian::native == std::endian::little ? "little" : "big";
    std::cout << "\n"
                 "All casts are assumed to be from "
              << endianness << "-endian to " << endianness
              << "-endian.\n"
                 "\n"
                 "Options\n"
                 "  -h        display this help and exit\n";

    std::exit(EXIT_SUCCESS);
}

[[noreturn]] void invalid_args(const std::string& err)
{
    if (!err.empty())
        std::cerr << err << '\n';
    xph::die("Try '", xph::exec_name, " -h' for more information.\n");
}

void assert_argc(int n, int argc)
{
    xph::die_if(argc != n, "expected ", n, " cast arguments, got ", argc);
}

void cast_bitset_to_character(int argc, [[maybe_unused]] char** argv)
{
    assert_argc(0, argc);

    char buf[8];
    ssize_t nread;

    while ((nread = read(STDIN_FILENO, buf, 8)) > 0) {
        xph::die_if(nread != 8, "could not read 8 ASCII bits");
        char c = 0;
        for (--nread; nread >= 0; --nread)
            c |= (buf[nread] & 1) << nread;
        write(STDOUT_FILENO, &c, sizeof(c));
    }
}

// FIXME: this function's output is sometimes wrong
void cast_character_to_bitset(int argc, [[maybe_unused]] char** argv)
{
    assert_argc(0, argc);

    unsigned char inbuf[PIPE_BUF];
    // TODO: use an outbuf[PIPE_BUF * 8]
    ssize_t nread;

    while ((nread = read(STDIN_FILENO, inbuf, PIPE_BUF)) > 0) {
        for (ssize_t i = 0; i < nread; ++i) {
            auto c = inbuf[i];
            for (int b = 0; b < 8; ++b)
                putchar(((c & (1 << b)) >> b) + '0');
        }
    }
}

template <typename T, auto converter, size_t size_override>
void cast_character_to_primitive(int argc, [[maybe_unused]] char** argv)
{
    assert_argc(0, argc);

    T buf;
    ssize_t nread;

    while ((nread = read(STDIN_FILENO, &buf, size_override)) > 0) {
        xph::die_if(nread != size_override,
                    "could not read ",
                    size_override,
                    " bytes for the specified type");
        std::cout << converter << buf << '\n';
    }
}

template <typename T, auto converter>
void cast_character_to_primitive(int argc, [[maybe_unused]] char** argv)
{
    assert_argc(0, argc);

    T buf;
    ssize_t nread;

    while ((nread = read(STDIN_FILENO, &buf, sizeof(buf))) > 0) {
        xph::die_if(
            nread != sizeof(buf), "could not read ", sizeof(buf), " bytes for the specified type");
        std::cout << converter << buf << '\n';
    }
}

template <typename T, auto converter>
void cast_character_to_xintx(int argc, char** argv)
{
    assert_argc(1, argc);

    T buf;
    size_t nbyte = xph::lexical_cast<decltype(*argv), decltype(nbyte)>(argv[0]);

    xph::die_if(!nbyte || nbyte > sizeof(buf),
                "cannot cast to ",
                nbyte,
                "-byte",
                nbyte == 1 ? "" : "s",
                "-long integers. minimum supported is 0. maximum supported is ",
                sizeof(buf),
                '.');

    ssize_t nread;
    buf = 0;
    while ((nread = read(STDIN_FILENO, &buf, nbyte)) > 0) {
        xph::die_if(static_cast<size_t>(nread) != nbyte,
                    "could not read ",
                    nbyte,
                    " bytes for the specified type");
        std::cout << converter << buf << '\n';
    }
}
