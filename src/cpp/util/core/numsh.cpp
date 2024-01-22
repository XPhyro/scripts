// @CXXFLAGS -funsafe-math-optimizations -fassociative-math -freciprocal-math -ffinite-math-only -fno-signed-zeros -fno-trapping-math
// @LDFLAGS -lboost_system

// TODO: enforce stuff in man page and help dialog

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>
#include <numeric>
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

#define FUNCTIONS                                                                        \
    X_FUNCTION(arange, 1, 2, "Append [MIN, MAX) to elements.")                           \
                                                                                         \
    X_FUNCTION(factor, 0, 0, "Map all elements to their factors.")                       \
                                                                                         \
    X_FUNCTION(difference, 0, 0, "Map all elements to their difference.")                \
    X_FUNCTION(partsum, 1, 1, "Map all elements to their partial sum.")                  \
    X_FUNCTION(partaltsum, 1, 1, "Map all elements to their partial alternating sum.")   \
    X_FUNCTION(partprod, 1, 1, "Map all elements to their partial product.")             \
                                                                                         \
    X_FUNCTION(cumsum, 0, 0, "Map all elements to their cumulative sum.")                \
    X_FUNCTION(cumaltsum, 0, 0, "Map all elements to their cumulative alternating sum.") \
    X_FUNCTION(cumprod, 0, 0, "Map all elements to their cumulative product.")           \
    X_FUNCTION(factorial,                                                                \
               0,                                                                        \
               0,                                                                        \
               "Map all elements to their factorial. "                                   \
               "If elements are not integers, truncate them first.")                     \
    X_FUNCTION(degrees, 0, 0, "Map all elements to their degrees.")                      \
    X_FUNCTION(radians, 0, 0, "Map all elements to their radians.")                      \
    X_FUNCTION(acos, 0, 0, "Map all elements to their acos. See `man 3 acos`.")          \
    X_FUNCTION(asin, 0, 0, "Map all elements to their asin. See `man 3 asin`.")          \
    X_FUNCTION(atan, 0, 0, "Map all elements to their atan. See `man 3 atan`.")          \
    X_FUNCTION(cbrt, 0, 0, "Map all elements to their cbrt. See `man 3 cbrt`.")          \
    X_FUNCTION(ceil, 0, 0, "Map all elements to their ceil. See `man 3 ceil`.")          \
    X_FUNCTION(cos, 0, 0, "Map all elements to their cos. See `man 3 cos`.")             \
    X_FUNCTION(cosh, 0, 0, "Map all elements to their cosh. See `man 3 cosh`.")          \
    X_FUNCTION(exp, 0, 0, "Map all elements to their exp. See `man 3 exp`.")             \
    X_FUNCTION(abs, 0, 0, "Map all elements to their abs. See `man 3 fabs`.")            \
    X_FUNCTION(floor, 0, 0, "Map all elements to their floor. See `man 3 floor`.")       \
    X_FUNCTION(log, 0, 0, "Map all elements to their log. See `man 3 log`.")             \
    X_FUNCTION(log10, 0, 0, "Map all elements to their log10. See `man 3 log10`.")       \
    X_FUNCTION(log2, 0, 0, "Map all elements to their log2. See `man 3 log2`.")          \
    X_FUNCTION(log1p, 0, 0, "Map all elements to their log1p.")                          \
    X_FUNCTION(pow, 1, 1, "Map all elements to their pow. See `man 3 pow`.")             \
    X_FUNCTION(round, 0, 0, "Map all elements to their round. See `man 3 round`.")       \
    X_FUNCTION(sin, 0, 0, "Map all elements to their sin. See `man 3 sin`.")             \
    X_FUNCTION(sinh, 0, 0, "Map all elements to their sinh. See `man 3 sinh`.")          \
    X_FUNCTION(sqrt, 0, 0, "Map all elements to their sqrt. See `man 3 sqrt`.")          \
    X_FUNCTION(tan, 0, 0, "Map all elements to their tan. See `man 3 tan`.")             \
    X_FUNCTION(tanh, 0, 0, "Map all elements to their tanh. See `man 3 tanh`.")          \
    X_FUNCTION(trunc, 0, 0, "Map all elements to their trunc. See `man 3 trunc`.")       \
                                                                                         \
    X_FUNCTION(zero, 0, 0, "Reduce to zero elements.")                                   \
    X_FUNCTION(nonzero, 0, 0, "Reduce to non-zero elements.")                            \
                                                                                         \
    X_FUNCTION(count, 0, 0, "Reduce to count of elements.")                              \
    X_FUNCTION(max, 0, 0, "Reduce to the maximum element.")                              \
    X_FUNCTION(min, 0, 0, "Reduce to the minimum element.")                              \
    X_FUNCTION(sum, 0, 0, "Reduce to the sum of the elements.")                          \
    X_FUNCTION(altsum, 0, 0, "Reduce to the alternating sum of the elements.")           \
    X_FUNCTION(product, 0, 0, "Reduce to the product of the elements.")                  \
    X_FUNCTION(mean, 0, 0, "Reduce to the mean of the elements.")                        \
    X_FUNCTION(std, 0, 0, "Reduce to the standard deviation of the elements.")           \
    X_FUNCTION(median, 0, 0, "Reduce to the median of the elements.")                    \
    X_FUNCTION(gmean, 0, 0, "Reduce to the geometric mean of the elements.")             \
    X_FUNCTION(hmean, 0, 0, "Reduce to the harmonic mean of the elements.")              \
    X_FUNCTION(gcd, 0, 0, "Reduce to the greatest common denominator of the elements.")

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
                       "With no NUMBER, read standard input unless -s is given. Empty lines are ignored\n"
                       "when reading standard input.\n"
                       "\n"
                       "Option -f must be given. If FUNC requires additional arguments,\n"
                       "option -p must be given exactly as many times as required.\n"
                       "\n"
                       "  -d  NUM   set the precision of output numbers. does not affect computation. default is "
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
    // none to many
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

    // one to many
    void factor([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        std::vector<double> factors;

        for (std::intmax_t num : nums) {
            if (!num)
                continue;

            if (num < 0)
                num = -num;

            while (!(num % 2)) {
                num /= 2;
                factors.push_back(2);
            }

            for (auto fac = 3; fac <= num; fac += 2) {
                while (!(num % fac)) {
                    num /= fac;
                    factors.push_back(fac);
                }
            }
        }

        nums.swap(factors);
    }

    // many to many
    void difference([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::difference(nums);
    }

    void partsum(std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::partsum(nums, argv[0]);
    }

    void partaltsum(std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::partaltsum(nums, argv[0]);
    }

    void partprod(std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::partprod(nums, argv[0]);
    }

    // one to one
    void cumsum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cumsum<double>(nums);
    }

    void cumaltsum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cumaltsum<double>(nums);
    }

    void cumprod([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cumprod<double>(nums);
    }

    void factorial([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
#if false // no gcc feature-test for fold_left
        xph::transform(nums, [](double num) {
            return std::ranges::fold_left(std::views::iota(1ull, static_cast<std::uintmax_t>(num) + 1ull),
                                          std::multiplies<>());
        });
#endif

        class factorial_impl final {
        public:
            static inline std::uintmax_t factorial(std::uintmax_t num)
            {
                std::uintmax_t fac = 1;
                for (auto&& i : std::views::iota(1ull, num + 1ull))
                    fac *= i;
                return fac;
            }
        };

        xph::transform(nums, [](double num) { return factorial_impl::factorial(num); });
    }

    void degrees([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::degrees<double>(nums);
    }

    void radians([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::radians<double>(nums);
    }

    void acos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)

    {
        xph::linalg::acos<double>(nums);
    }

    void asin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::asin<double>(nums);
    }

    void atan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::atan<double>(nums);
    }

    void cbrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cbrt<double>(nums);
    }

    void ceil([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::ceil<double>(nums);
    }

    void cos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cos<double>(nums);
    }

    void cosh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::cosh<double>(nums);
    }

    void exp([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::exp<double>(nums);
    }

    void abs([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::abs<double>(nums);
    }

    void floor([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::floor<double>(nums);
    }

    void log([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::log<double>(nums);
    }

    void log10([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::log10<double>(nums);
    }

    void log2([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::log2<double>(nums);
    }

    void log1p([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::log1p<double>(nums);
    }

    void pow(std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::pow<double>(nums, argv[0]);
    }

    void round([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::round<double>(nums);
    }

    void sin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::sin<double>(nums);
    }

    void sinh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::sinh<double>(nums);
    }

    void sqrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::sqrt<double>(nums);
    }

    void tan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::tan<double>(nums);
    }

    void tanh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::tanh<double>(nums);
    }

    void trunc([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::linalg::trunc<double>(nums);
    }

    // one to optional one
    void zero([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        std::erase_if(nums, [](double num) { return !xph::approx_zero(num); });
    }

    void nonzero([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        std::erase_if(nums, [](double num) { return xph::approx_zero(num); });
    }

    // many to one
    void count([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { static_cast<double>(nums.size()) };
    }

    void max([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { *std::max_element(nums.begin(), nums.end()) };
    }

    void min([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { *std::min_element(nums.begin(), nums.end()) };
    }

    void sum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 0.0) };
    }

    void altsum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 0.0, [&](double altsum, double num) {
            static double sign = -1.0;
            return altsum + num * (sign = -sign);
        }) };
    }

    void product([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 1.0, std::multiplies<>()) };
    }

    void mean([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 0.0) / nums.size() };
    }

    void std([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        double mean = std::accumulate(nums.begin(), nums.end(), 0.0) / nums.size();
        nums = { std::sqrt(std::accumulate(nums.begin(),
                                           nums.end(),
                                           0.0,
                                           [&](double accum, double num) {
                                               double diff = num - mean;
                                               return accum + diff * diff;
                                           }) /
                           (nums.size() - 1)) };
    }

    void median([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        std::sort(nums.begin(), nums.end());
        nums = { nums.size() % 2 ? nums[nums.size() / 2] :
                                   (nums[nums.size() / 2 - 1] + nums[nums.size() / 2]) / 2 };
    }

    void gmean([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::pow(std::accumulate(nums.begin(),
                                          nums.end(),
                                          1.0,
                                          [&](double accum, double num) { return accum * num; }),
                          1.0 / nums.size()) };
    }

    void hmean([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { nums.size() /
                 std::accumulate(nums.begin(), nums.end(), 0.0, [&](double accum, double num) {
                     return accum + 1.0 / num;
                 }) };
    }

    void gcd([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::min();

        for (double num : nums) {
            num = std::abs(num);
            if (num < min)
                min = num;
            if (num > max)
                max = num;
        }

        class gcd_impl final {
        public:
            static inline std::uintmax_t gcd_pair(std::uintmax_t min, std::uintmax_t max)
            {
                return !max ? min : gcd_pair(max, min % max);
            }
        };

        nums = { static_cast<double>(gcd_impl::gcd_pair(std::abs(min), std::abs(max))) };
    }
} // namespace func
