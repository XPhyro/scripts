// @CXXFLAGS -funsafe-math-optimizations -fassociative-math -freciprocal-math -ffinite-math-only -fno-signed-zeros -fno-trapping-math
// @LDFLAGS -lboost_system

// TODO: enforce stuff in man page and help dialog

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
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
    xph::gather_exec_info(argc, argv);

    std::vector<function> sel_funcs;
    std::vector<std::vector<double>> func_argvs;

    bool opt_multi = false;
    bool opt_nostdin = false;

    for (int i; (i = getopt(argc, argv, "f:hLmp:s")) != -1;) {
        switch (i) {
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
        const char* err = func_argv.size() < sel_func.min_argc ? "few" :
                          func_argv.size() > sel_func.max_argc ? "many" :
                                                                 nullptr;

        if (err) {
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
            std::cout << num << '\n';
    } else {
        for (const auto&& [func, func_argv] : boost::combine(sel_funcs, func_argvs)) {
            auto nums_dup = nums;

            func.function(func_argv, nums_dup);

            for (const auto& num : nums_dup)
                std::cout << num << '\n';
        }
    }

    return 0;
}

namespace func {
    // none to many
    void arange(std::span<double> argv, std::vector<double>& nums)
    {
        std::intmax_t min, max;
        switch (argv.size()) {
            case 1:
                min = 0;
                max = argv[0];
                break;
            case 2:
                min = argv[0];
                max = argv[1];
                break;
            default:
                return;
        }

        nums.reserve(nums.size() + max - min);
        for (auto&& i : std::views::iota(min, max))
            nums.push_back(i);
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
        std::vector<double> difference;

        difference.reserve(nums.size() - 1);
        for (auto&& window : nums | std::views::slide(2))
            difference.push_back(window[1] - window[0]);

        nums.swap(difference);
    }

    void partsum(std::span<double> argv, std::vector<double>& nums)
    {
        std::vector<double> partsum;
        partsum.reserve(nums.size() - argv[0] + 1);
        for (const auto& window : nums | std::views::slide(argv[0]))
            partsum.push_back(std::accumulate(window.begin(), window.end(), 0.0));
        nums.swap(partsum);
    }

    void partaltsum(std::span<double> argv, std::vector<double>& nums)
    {
        std::vector<double> partaltsum;
        partaltsum.reserve(nums.size() - argv[0] + 1);
        for (const auto& window : nums | std::views::slide(argv[0])) {
            partaltsum.push_back(std::accumulate(
                window.begin(), window.end(), 0.0, [&](double cumaltsum, double num) {
                    static double sign = -1.0;
                    return cumaltsum + num * (sign = -sign);
                }));
        }
        nums.swap(partaltsum);
    }

    void partprod(std::span<double> argv, std::vector<double>& nums)
    {
        std::vector<double> partprod;
        partprod.reserve(nums.size() - argv[0] + 1);
        for (const auto& window : nums | std::views::slide(argv[0]))
            partprod.push_back(
                std::accumulate(window.begin(), window.end(), 1.0, std::multiplies<>()));
        nums.swap(partprod);
    }

    // one to one
    void cumsum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        double cumsum = 0.0;
        for (double& num : nums)
            num = cumsum += num;
    }

    void cumaltsum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        double cumaltsum = 0.0;
        double sign = -1.0;
        for (double& num : nums)
            num = cumaltsum += num * (sign = -sign);
    }

    void cumprod([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        double cumprod = 1.0;
        for (double& num : nums)
            num = cumprod *= num;
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
        xph::transform(nums, [](double num) { return num * 180.0 / std::numbers::pi; });
    }

    void radians([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return num * std::numbers::pi / 180.0; });
    }

    void acos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)

    {
        xph::transform(nums, [](double num) { return std::acos(num); });
    }

    void asin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::asin(num); });
    }

    void atan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::atan(num); });
    }

    void cbrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::cbrt(num); });
    }

    void ceil([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::ceil(num); });
    }

    void cos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::cos(num); });
    }

    void cosh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::cosh(num); });
    }

    void exp([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::exp(num); });
    }

    void abs([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::fabs(num); });
    }

    void floor([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::floor(num); });
    }

    void log([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::log(num); });
    }

    void log10([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::log10(num); });
    }

    void log2([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::log2(num); });
    }

    void log1p([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::log(1.0 + num); });
    }

    void pow(std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [&](double num) { return std::pow(num, argv[0]); });
    }

    void round([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::round(num); });
    }

    void sin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::sin(num); });
    }

    void sinh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::sinh(num); });
    }

    void sqrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::sqrt(num); });
    }

    void tan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::tan(num); });
    }

    void tanh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::tanh(num); });
    }

    void trunc([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        xph::transform(nums, [](double num) { return std::trunc(num); });
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
