// @CXXFLAGS -funsafe-math-optimizations -fassociative-math -freciprocal-math -ffinite-math-only -fno-signed-zeros -fno-trapping-math
// @LDFLAGS -lboost_system

// TODO: enforce stuff in man page and help dialog

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ranges>
#include <span>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <version>

#include <unistd.h>

#include <boost/range/combine.hpp>

#include <xph/algorithm.hpp>
#include <xph/die.hpp>
#include <xph/functional.hpp>
#include <xph/lexical_cast.hpp>
#include <xph/linalg.hpp>
#include <xph/math.hpp>

DEFINE_EXEC_INFO();

#define X_FUNCTION_VECTOR_0(FUNC, MIN, MAX, DESC) \
    X_FUNCTION(FUNC, MIN, MAX, DESC) Y_FUNCTION_VECTOR_0(FUNC)
#define X_FUNCTION_VECTOR_1(FUNC, MIN, MAX, DESC) \
    X_FUNCTION(FUNC, MIN, MAX, DESC) Y_FUNCTION_VECTOR_1(FUNC)
#define X_FUNCTION_SPAN_0(FUNC, MIN, MAX, DESC) \
    X_FUNCTION(FUNC, MIN, MAX, DESC) Y_FUNCTION_SPAN_0(FUNC)
#define X_FUNCTION_SPAN_1(FUNC, MIN, MAX, DESC) \
    X_FUNCTION(FUNC, MIN, MAX, DESC) Y_FUNCTION_SPAN_1(FUNC)

#define FUNCTIONS                                                                                  \
    /* none to many */                                                                             \
    X_FUNCTION(arange, 1, 3, "Append [MIN, MAX) in STEP steps to elements.")                       \
                                                                                                   \
    /* one to many */                                                                              \
    X_FUNCTION_VECTOR_0(factor, 0, 0, "Map all elements to their factors.")                        \
                                                                                                   \
    /* many to many */                                                                             \
    X_FUNCTION_VECTOR_0(difference, 0, 0, "Map all elements to their difference.")                 \
    X_FUNCTION_VECTOR_1(partsum, 1, 1, "Map all elements to their partial sum.")                   \
    X_FUNCTION_VECTOR_1(partaltsum, 1, 1, "Map all elements to their partial alternating sum.")    \
    X_FUNCTION_VECTOR_1(partprod, 1, 1, "Map all elements to their partial product.")              \
                                                                                                   \
    /* one to one */                                                                               \
    X_FUNCTION_SPAN_1(add, 1, 1, "Add the given number to all elements.")                          \
    X_FUNCTION_SPAN_1(subtract, 1, 1, "Subtract the given number from all elements.")              \
    X_FUNCTION_SPAN_1(multiply, 1, 1, "Multiply all elements with the given number.")              \
    X_FUNCTION_SPAN_1(divide, 1, 1, "Divide all elements by the given number.")                    \
    X_FUNCTION_SPAN_1(                                                                             \
        equals, 1, 1, "Map all elements to their equality status with the given number.")          \
    X_FUNCTION_SPAN_1(                                                                             \
        not_equals, 1, 1, "Map all elements to their non-equality status with the given number.")  \
    X_FUNCTION_SPAN_1(                                                                             \
        lesser, 1, 1, "Map all elements to their relation status with the given number.")          \
    X_FUNCTION_SPAN_1(                                                                             \
        greater, 1, 1, "Map all elements to their relation status with the given number.")         \
    X_FUNCTION_SPAN_1(                                                                             \
        lesser_or_equal, 1, 1, "Map all elements to their relation status with the given number.") \
    X_FUNCTION_SPAN_1(greater_or_equal,                                                            \
                      1,                                                                           \
                      1,                                                                           \
                      "Map all elements to their relation status with the given number.")          \
    X_FUNCTION_SPAN_0(logical_not, 0, 0, "Map all elements to their logical inversion.")           \
    X_FUNCTION_SPAN_0(bitwise_not, 0, 0, "Map all elements to their bitwise inversion.")           \
    X_FUNCTION_SPAN_0(                                                                             \
        unsigned_bitwise_not, 0, 0, "Map all elements to their unsigned bitwise inversion.")       \
    X_FUNCTION_SPAN_0(cumsum, 0, 0, "Map all elements to their cumulative sum.")                   \
    X_FUNCTION_SPAN_0(cumaltsum, 0, 0, "Map all elements to their cumulative alternating sum.")    \
    X_FUNCTION_SPAN_0(cumprod, 0, 0, "Map all elements to their cumulative product.")              \
    X_FUNCTION_SPAN_0(factorial,                                                                   \
                      0,                                                                           \
                      0,                                                                           \
                      "Map all elements to their factorial. "                                      \
                      "If elements are not integers, truncate them first.")                        \
    X_FUNCTION_SPAN_0(degrees, 0, 0, "Map all elements to their degrees.")                         \
    X_FUNCTION_SPAN_0(radians, 0, 0, "Map all elements to their radians.")                         \
    X_FUNCTION_SPAN_0(acos, 0, 0, "Map all elements to their acos. See `man 3 acos`.")             \
    X_FUNCTION_SPAN_0(asin, 0, 0, "Map all elements to their asin. See `man 3 asin`.")             \
    X_FUNCTION_SPAN_0(atan, 0, 0, "Map all elements to their atan. See `man 3 atan`.")             \
    X_FUNCTION_SPAN_0(cbrt, 0, 0, "Map all elements to their cbrt. See `man 3 cbrt`.")             \
    X_FUNCTION_SPAN_0(ceil, 0, 0, "Map all elements to their ceil. See `man 3 ceil`.")             \
    X_FUNCTION_SPAN_0(cos, 0, 0, "Map all elements to their cos. See `man 3 cos`.")                \
    X_FUNCTION_SPAN_0(cosh, 0, 0, "Map all elements to their cosh. See `man 3 cosh`.")             \
    X_FUNCTION_SPAN_0(exp, 0, 0, "Map all elements to their exp. See `man 3 exp`.")                \
    X_FUNCTION_SPAN_0(abs, 0, 0, "Map all elements to their abs. See `man 3 fabs`.")               \
    X_FUNCTION_SPAN_0(floor, 0, 0, "Map all elements to their floor. See `man 3 floor`.")          \
    X_FUNCTION_SPAN_0(log, 0, 0, "Map all elements to their log. See `man 3 log`.")                \
    X_FUNCTION_SPAN_0(log10, 0, 0, "Map all elements to their log10. See `man 3 log10`.")          \
    X_FUNCTION_SPAN_0(log2, 0, 0, "Map all elements to their log2. See `man 3 log2`.")             \
    X_FUNCTION_SPAN_0(log1p, 0, 0, "Map all elements to their log1p.")                             \
    X_FUNCTION_SPAN_1(pow, 1, 1, "Map all elements to their pow. See `man 3 pow`.")                \
    X_FUNCTION_SPAN_0(round, 0, 0, "Map all elements to their round. See `man 3 round`.")          \
    X_FUNCTION_SPAN_0(sin, 0, 0, "Map all elements to their sin. See `man 3 sin`.")                \
    X_FUNCTION_SPAN_0(sinh, 0, 0, "Map all elements to their sinh. See `man 3 sinh`.")             \
    X_FUNCTION_SPAN_0(sqrt, 0, 0, "Map all elements to their sqrt. See `man 3 sqrt`.")             \
    X_FUNCTION_SPAN_0(tan, 0, 0, "Map all elements to their tan. See `man 3 tan`.")                \
    X_FUNCTION_SPAN_0(tanh, 0, 0, "Map all elements to their tanh. See `man 3 tanh`.")             \
    X_FUNCTION_SPAN_0(trunc, 0, 0, "Map all elements to their trunc. See `man 3 trunc`.")          \
                                                                                                   \
    /* one to optional one */                                                                      \
    X_FUNCTION_VECTOR_0(zero, 0, 0, "Reduce to zero elements.")                                    \
    X_FUNCTION_VECTOR_0(nonzero, 0, 0, "Reduce to non-zero elements.")                             \
                                                                                                   \
    /* many to one */                                                                              \
    X_FUNCTION_VECTOR_0(count, 0, 0, "Reduce to count of elements.")                               \
    X_FUNCTION_VECTOR_0(max, 0, 0, "Reduce to the maximum element.")                               \
    X_FUNCTION_VECTOR_0(min, 0, 0, "Reduce to the minimum element.")                               \
    X_FUNCTION_VECTOR_0(sum, 0, 0, "Reduce to the sum of the elements.")                           \
    X_FUNCTION_VECTOR_0(altsum, 0, 0, "Reduce to the alternating sum of the elements.")            \
    X_FUNCTION_VECTOR_0(product, 0, 0, "Reduce to the product of the elements.")                   \
    X_FUNCTION_VECTOR_0(mean, 0, 0, "Reduce to the mean of the elements.")                         \
    X_FUNCTION_VECTOR_0(std, 0, 0, "Reduce to the standard deviation of the elements.")            \
    X_FUNCTION_VECTOR_0(median, 0, 0, "Reduce to the median of the elements.")                     \
    X_FUNCTION_VECTOR_0(gmean, 0, 0, "Reduce to the geometric mean of the elements.")              \
    X_FUNCTION_VECTOR_0(hmean, 0, 0, "Reduce to the harmonic mean of the elements.")               \
    X_FUNCTION_VECTOR_0(gcd, 0, 0, "Reduce to the greatest common denominator of the elements.")

#define Y_FUNCTION_VECTOR_0(FUNC)
#define Y_FUNCTION_VECTOR_1(FUNC)
#define Y_FUNCTION_SPAN_0(FUNC)
#define Y_FUNCTION_SPAN_1(FUNC)

#define X_FUNCTION(FUNC, MIN, MAX, DESC) \
    static_assert(MIN >= 0);             \
    static_assert(MIN <= MAX);           \
    FUNCTIONS
#undef X_FUNCTION

#define X_FUNCTION(FUNC, MIN, MAX, DESC) \
    void FUNC(std::span<double> argv, std::vector<double>& nums);

namespace func {
    FUNCTIONS
} // namespace func

#undef X_FUNCTION

struct function {
public:
    std::string description;
    std::function<void(std::span<double>, std::vector<double>&)> function;
    std::size_t min_argc;
    std::size_t max_argc;
};

template <typename Tret>
constexpr Tret get_functions(void) noexcept
{
#define X_FUNCTION(FUNC, MIN, MAX, DESC) { #FUNC, { DESC, func::FUNC, MIN, MAX } },
    return { FUNCTIONS };
#undef X_FUNCTION
}

#undef Y_FUNCTION_VECTOR_0
#undef Y_FUNCTION_VECTOR_1
#undef Y_FUNCTION_SPAN_0
#undef Y_FUNCTION_SPAN_1

std::unordered_map<std::string_view, function> functions =
    get_functions<std::unordered_map<std::string_view, function>>();

int main(int argc, char* argv[])
{
    const constexpr int min_precision = 1;
    const constexpr int default_precision = 6;
    const constexpr int max_precision = std::numeric_limits<double>::digits10 + 1;

    xph::gather_exec_info(argc, argv);

    std::vector<function> sel_funcs;
    std::vector<std::vector<double>> func_argvs;

    bool opt_multi = false;
    bool opt_nostdin = false;
    int opt_precision = default_precision;

    for (int i; (i = getopt(argc, argv, "d:f:hLmp:s")) != -1;) {
        switch (i) {
            case 'd': {
                opt_precision = xph::lexical_cast<char*, int>(optarg);
                if (opt_precision < min_precision || opt_precision > max_precision)
                    xph::die("invalid precision given: ", optarg);
                break;
            }
            case 'f': {
                function func;
                try {
                    func = functions.at(optarg);
                } catch (const std::out_of_range& e) {
                    xph::die("invalid function given: ", optarg);
                }

                sel_funcs.push_back(func);
                func_argvs.emplace_back();
                func_argvs.back().reserve(func.max_argc);
                break;
            }
            case 'h': {
                std::cout
                    << "Usage: " << xph::exec_path
                    << " [OPTION]... [NUMBER]...\n"
                       "Do mathematical calculations on all given numbers.\n"
                       "\n"
                       "With no NUMBER, read standard input unless -s is given. Empty lines are\n"
                       "ignored when reading standard input.\n"
                       "\n"
                       "Option -f must be given. If FUNC requires additional arguments,\n"
                       "option -p must be given exactly as many times as required.\n"
                       "\n"
                       "  -a  NUM   enable arbitrary precision computation with NUM figures.\n"
                       "            will hinder performance by many orders of magnitudes.\n"
                       "  -d  NUM   set the precision of output numbers. does not affect computation.\n"
                       "            default is "
                    << default_precision << ", range is [" << min_precision << ", " << max_precision
                    << "].\n"
                       "  -f  FUNC  function to pass numbers through. pass -L to see the list.\n"
                       "  -h        display this help and exit\n"
                       "  -L        list all supported functions and exit\n"
                       "  -m        do not chain functions, output multiple results instead\n"
                       "  -p  ARG   pass additional arguments to FUNC. this option can be\n"
                       "            given multiple times, and has to be given after -f.\n"
                       "  -s        do not read numbers from standard input\n";
                return EXIT_SUCCESS;
            }
            case 'L': {
                for (const auto& [name, func] :
                     get_functions<std::vector<std::pair<std::string_view, function>>>())
                    std::cout << name << ":\n\t" << func.description << '\n';

                return EXIT_SUCCESS;
            }
            case 'm': {
                opt_multi = true;
                break;
            }
            case 'p': {
                func_argvs.back().push_back(xph::lexical_cast<char*, double>(optarg));
                break;
            }
            case 's': {
                opt_nostdin = true;
                break;
            }
            default: {
                std::cout << "Try '" << xph::exec_path << " -h' for more information.\n";
                return EXIT_FAILURE;
            }
        }
    }

    argc -= optind;
    argv += optind;

    for (const auto&& [sel_func, func_argv] : boost::combine(sel_funcs, func_argvs)) {
        if (const char* err = func_argv.size() < sel_func.min_argc ? "few" :
                              func_argv.size() > sel_func.max_argc ? "many" :
                                                                     nullptr;
            err) {
            // this is very inefficient, but we die anyways
            std::string func_name = "NULL";
            for (const auto& [name, func] : functions) {
                if (xph::function_eq(func.function, sel_func.function)) {
                    func_name = name;
                    break;
                }
            }

            xph::die("too ", err, " parameters given to function ", func_name);
        }
    }

    std::vector<double> nums;
    if (argc) {
        std::ranges::copy(std::views::counted(argv, argc) |
                              std::views::transform([&](const auto& arg) {
                                  return xph::lexical_cast<char*, double>(arg);
                              }),
                          std::back_inserter(nums));
    } else if (!opt_nostdin) {
        for (double num; std::cin >> num; nums.push_back(num)) {}
    }

    if (!opt_multi) {
        for (const auto&& [func, func_argv] : boost::combine(sel_funcs, func_argvs))
            func.function(func_argv, nums);

        for (const auto& num : nums)
            std::cout << std::setprecision(opt_precision) << num << '\n';
    } else {
        decltype(nums) nums_dup;
        nums_dup.reserve(nums.size());
        for (const auto&& [func, func_argv] : boost::combine(sel_funcs, func_argvs)) {
            std::copy(nums.begin(), nums.end(), std::back_inserter(nums_dup));

            func.function(func_argv, nums_dup);

            for (const auto& num : nums_dup)
                std::cout << std::setprecision(opt_precision) << num << '\n';

            nums_dup.clear();
        }
    }

    return 0;
}

namespace func {
    void arange(std::span<double> argv, std::vector<double>& nums)
    {
        double start, stop, step;
        switch (argv.size()) {
            case 1:
                start = 0;
                stop = argv[0];
                step = 1;
                break;
            case 2:
                start = argv[0];
                stop = argv[1];
                step = 1;
                break;
            case 3:
                start = argv[0];
                stop = argv[1];
                step = argv[2];
                break;
            default:
                return;
        }
        xph::linalg::arange<double>(start, stop, step, nums);
    }

#define X_FUNCTION(FUNC, MIN, MAX, DESC)

#define Y_FUNCTION_SPAN_0(FUNC)                                                   \
    void FUNC([[maybe_unused]] std::span<double> argv, std::vector<double>& nums) \
    {                                                                             \
        xph::linalg::FUNC<double>(nums);                                          \
    }

#define Y_FUNCTION_SPAN_1(FUNC)                                  \
    void FUNC(std::span<double> argv, std::vector<double>& nums) \
    {                                                            \
        xph::linalg::FUNC<double>(nums, argv[0]);                \
    }

#define Y_FUNCTION_VECTOR_0(FUNC)                                                 \
    void FUNC([[maybe_unused]] std::span<double> argv, std::vector<double>& nums) \
    {                                                                             \
        xph::linalg::FUNC(nums);                                                  \
    }

#define Y_FUNCTION_VECTOR_1(FUNC)                                \
    void FUNC(std::span<double> argv, std::vector<double>& nums) \
    {                                                            \
        xph::linalg::FUNC(nums, argv[0]);                        \
    }

    FUNCTIONS

#undef X_FUNCTION
#undef Y_FUNCTION_VECTOR_0
#undef Y_FUNCTION_VECTOR_1
#undef Y_FUNCTION_SPAN_0
#undef Y_FUNCTION_SPAN_1
} // namespace func
