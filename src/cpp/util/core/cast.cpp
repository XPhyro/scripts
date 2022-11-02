// C++
#include <algorithm>
#include <bitset>
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
    oct8,
    oct16,
    oct32,
    oct64,
    uint8,
    uint16,
    uint32,
    uint64,
};

[[noreturn]] void help();
[[noreturn]] void invalidargs(const std::string& err);
void cast_bitset_to_character();
void cast_character_to_bitset();
template <typename T>
void cast_character_to_primitive(auto converter);
template <typename T>
void cast_character_to_dec();
template <typename T>
void cast_character_to_float_hex();
template <typename T>
void cast_character_to_int_hex();
template <typename T>
void cast_character_to_int_oct();

const std::unordered_map<std::string, type> types = {
    {"char",         type::character  },
    { "float32",     type::float32    },
    { "float64",     type::float64    },
    { "float128",    type::float128   },
    { "hex8",        type::hex8       },
    { "hex16",       type::hex16      },
    { "hex32",       type::hex32      },
    { "hex64",       type::hex64      },
    { "hexfloat32",  type::hexfloat32 },
    { "hexfloat64",  type::hexfloat64 },
    { "hexfloat128", type::hexfloat128},
    { "int8",        type::int8       },
    { "int16",       type::int16      },
    { "int32",       type::int32      },
    { "int64",       type::int64      },
    { "oct8",        type::oct8       },
    { "oct16",       type::oct16      },
    { "oct32",       type::oct32      },
    { "oct64",       type::oct64      },
    { "uint8",       type::uint8      },
    { "uint16",      type::uint16     },
    { "uint32",      type::uint32     },
    { "uint64",      type::uint64     },
    { "bit",         type::bitset     },
};

const std::unordered_map<std::tuple<type, type>, std::function<void()>> funcs = {
    {{ type::bitset, type::character },       cast_bitset_to_character                },
    { { type::character, type::bitset },      cast_character_to_bitset                },
    { { type::character, type::float32 },     cast_character_to_dec<float>            },
    { { type::character, type::float64 },     cast_character_to_dec<double>           },
    { { type::character, type::float128 },    cast_character_to_dec<long double>      },
    { { type::character, type::hex8 },        cast_character_to_int_hex<int8_t>       },
    { { type::character, type::hex16 },       cast_character_to_int_hex<int16_t>      },
    { { type::character, type::hex32 },       cast_character_to_int_hex<int32_t>      },
    { { type::character, type::hex64 },       cast_character_to_int_hex<int64_t>      },
    { { type::character, type::hexfloat32 },  cast_character_to_float_hex<float>      },
    { { type::character, type::hexfloat64 },  cast_character_to_float_hex<double>     },
    { { type::character, type::hexfloat128 }, cast_character_to_float_hex<long double>},
    { { type::character, type::int8 },        cast_character_to_dec<int8_t>           },
    { { type::character, type::int16 },       cast_character_to_dec<int16_t>          },
    { { type::character, type::int32 },       cast_character_to_dec<int32_t>          },
    { { type::character, type::int64 },       cast_character_to_dec<int64_t>          },
    { { type::character, type::oct8 },        cast_character_to_int_oct<int8_t>       },
    { { type::character, type::oct16 },       cast_character_to_int_oct<int16_t>      },
    { { type::character, type::oct32 },       cast_character_to_int_oct<int32_t>      },
    { { type::character, type::oct64 },       cast_character_to_int_oct<int64_t>      },
    { { type::character, type::uint8 },       cast_character_to_dec<uint8_t>          },
    { { type::character, type::uint16 },      cast_character_to_dec<uint16_t>         },
    { { type::character, type::uint32 },      cast_character_to_dec<uint32_t>         },
    { { type::character, type::uint64 },      cast_character_to_dec<uint64_t>         },
};

const char* execname;

int main(int argc, char* argv[])
{
    CAPTURE_EXECNAME();

    for (int i; (i = getopt(argc, argv, "h")) != -1;) {
        switch (i) {
            case 'h':
                help();
            default:
                invalidargs(xph::consts::str::empty);
        }
    }

    argv += optind;
    argc -= optind;

    if (argc < 2)
        invalidargs(xph::consts::str::empty);

    std::function<void()> func;
    try {
        auto fromtype = types.at(xph::str::makelower(std::string(argv[0])));
        auto totype = types.at(xph::str::makelower(std::string(argv[1])));
        func = funcs.at({ fromtype, totype });
    } catch (const std::out_of_range& e) {
        invalidargs("invalid type or type pair given");
    }

    argv += 2;
    argc -= 2;

    if (argc)
        invalidargs("extra operands given");

    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    func();
}

[[noreturn]] void help()
{
    std::cout << "Usage: " << execname
              << " [OPTION...] [FROM_TYPE] [TO_TYPE]\n"
                 "Cast types to types.\n"
                 "\n"
                 "Valid types are: ";

    std::vector<std::string> typenames;
    typenames.reserve(types.size());
    for (const auto& [name, _] : types)
        typenames.push_back(name);

    std::sort(typenames.begin(), typenames.end());

    for (auto it = typenames.begin(); it < typenames.end() - 1; ++it)
        std::cout << *it << ", ";
    std::cout << typenames.back() << ".\n";

    // TODO: print possible pairs

    std::cout << "\n"
                 "Options\n"
                 "  -h        display this help and exit\n";

    std::exit(EXIT_SUCCESS);
}

[[noreturn]] void invalidargs(const std::string& err)
{
    if (!err.empty())
        std::cerr << err << '\n';
    xph::die("Try '", execname, " -h' for more information.\n");
}

void cast_bitset_to_character()
{
    char buf[8];
    ssize_t n;

    while ((n = read(STDIN_FILENO, buf, 8)) > 0) {
        if (n != 8)
            xph::die("could not read 8 ASCII bits");
        char c = 0;
        for (const auto&& i : std::views::iota(0, n - 1))
            c |= (buf[i] & 1) << i;
        write(STDOUT_FILENO, &c, sizeof(c));
    }
}

// FIXME: this function's output is sometimes wrong
void cast_character_to_bitset()
{
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

template <typename T>
void cast_character_to_x(auto converter)
{
    T buf;
    ssize_t n;

    while ((n = read(STDIN_FILENO, &buf, sizeof(buf))) > 0) {
        if (n != sizeof(buf))
            xph::die("could not read ", sizeof(buf), " bytes for the specified type");
        std::cout << converter << buf << '\n';
    }
}

template <typename T>
void cast_character_to_dec()
{
    cast_character_to_x<T>(std::dec);
}

template <typename T>
void cast_character_to_float_hex()
{
    cast_character_to_x<T>(std::hexfloat);
}

template <typename T>
void cast_character_to_int_hex()
{
    cast_character_to_x<T>(std::hex);
}

template <typename T>
void cast_character_to_int_oct()
{
    cast_character_to_x<T>(std::oct);
}
