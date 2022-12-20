// @LDFLAGS -lboost_system

// TODO: read stdin
// TODO: sort -L alphabetically
// TODO: enforce stuff in man page and help dialog
// TODO: do not enforce "-f must be given"

#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <span>
#include <unordered_map>
#include <vector>

#include <unistd.h>

#include <boost/range/combine.hpp>

#include <die.hpp>
#include <lexical_cast.hpp>

#include <mathutil.h>

DEFINE_EXEC_INFO();

#define DECL_FUNC(FUNC_NAME) void FUNC_NAME(std::span<double> argv, std::vector<double>& nums);

namespace func {
    DECL_FUNC(acos);
    DECL_FUNC(asin);
    DECL_FUNC(atan);
    DECL_FUNC(cbrt);
    DECL_FUNC(ceil);
    DECL_FUNC(cos);
    DECL_FUNC(cosh);
    DECL_FUNC(exp);
    DECL_FUNC(fabs);
    DECL_FUNC(floor);
    DECL_FUNC(log);
    DECL_FUNC(log10);
    DECL_FUNC(log2);
    DECL_FUNC(pow);
    DECL_FUNC(round);
    DECL_FUNC(sin);
    DECL_FUNC(sinh);
    DECL_FUNC(sqrt);
    DECL_FUNC(tan);
    DECL_FUNC(tanh);
    DECL_FUNC(trunc);

    DECL_FUNC(nonzero);

    DECL_FUNC(max);
    DECL_FUNC(min);
    DECL_FUNC(sum);
    DECL_FUNC(mean);
} // namespace func

struct function {
public:
    std::string description;
    std::function<void(std::span<double>, std::vector<double>&)> function;
    std::size_t min_argc;
    std::size_t max_argc;
};

std::unordered_map<std::string_view, function> functions = {
    { "acos", { "Map all elements to their acos. See `man 3 acos`.", func::acos, 0, 0 } },
    { "asin", { "Map all elements to their asin. See `man 3 asin`.", func::asin, 0, 0 } },
    { "atan", { "Map all elements to their atan. See `man 3 atan`.", func::atan, 0, 0 } },
    { "cbrt", { "Map all elements to their cbrt. See `man 3 cbrt`.", func::cbrt, 0, 0 } },
    { "ceil", { "Map all elements to their ceil. See `man 3 ceil`.", func::ceil, 0, 0 } },
    { "cos", { "Map all elements to their cos. See `man 3 cos`.", func::cos, 0, 0 } },
    { "cosh", { "Map all elements to their cosh. See `man 3 cosh`.", func::cosh, 0, 0 } },
    { "exp", { "Map all elements to their exp. See `man 3 exp`.", func::exp, 0, 0 } },
    { "abs", { "Map all elements to their fabs. See `man 3 fabs`.", func::fabs, 0, 0 } },
    { "floor", { "Map all elements to their floor. See `man 3 floor`.", func::floor, 0, 0 } },
    { "log", { "Map all elements to their log. See `man 3 log`.", func::log, 0, 0 } },
    { "log10", { "Map all elements to their log10. See `man 3 log10`.", func::log10, 0, 0 } },
    { "log2", { "Map all elements to their log2. See `man 3 log2`.", func::log2, 0, 0 } },
    { "pow", { "Map all elements to their pow. See `man 3 pow`.", func::pow, 1, 1 } },
    { "round", { "Map all elements to their round. See `man 3 round`.", func::round, 0, 0 } },
    { "sin", { "Map all elements to their sin. See `man 3 sin`.", func::sin, 0, 0 } },
    { "sinh", { "Map all elements to their sinh. See `man 3 sinh`.", func::sinh, 0, 0 } },
    { "sqrt", { "Map all elements to their sqrt. See `man 3 sqrt`.", func::sqrt, 0, 0 } },
    { "tan", { "Map all elements to their tan. See `man 3 tan`.", func::tan, 0, 0 } },
    { "tanh", { "Map all elements to their tanh. See `man 3 tanh`.", func::tanh, 0, 0 } },
    { "trunc", { "Map all elements to their trunc. See `man 3 trunc`.", func::trunc, 0, 0 } },
    { "nonzero", { "Reduce to non-zero elements.", func::nonzero, 0, 0 } },
    { "max", { "Reduce to the maximum element.", func::max, 0, 0 } },
    { "min", { "Reduce to the minimum element.", func::min, 0, 0 } },
    { "sum", { "Reduce to the sum of the elements.", func::sum, 0, 0 } },
    { "mean", { "Reduce to the mean of the elements.", func::mean, 0, 0 } },
};

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    std::vector<function> sel_funcs;
    std::vector<std::vector<double>> func_argvs;

    for (int i; (i = getopt(argc, argv, "f:hLp:")) != -1;) {
        switch (i) {
            case 'f': {
                if (!sel_funcs.empty() && func_argvs.back().size() < sel_funcs.back().min_argc)
                    xph::die("too few parameters given to function ",
                             "" /* TODO: print function name */
                    );
                if (!functions.contains(optarg))
                    xph::die("invalid function given: ", optarg);
                auto func = functions.at(optarg);
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
                       "With no NUMBER, read standard input. Empty lines are ignored\n"
                       "when reading standard input.\n"
                       "\n"
                       "Option -f must be given. If FUNC requires additional arguments,\n"
                       "option -o must be given exactly as many times as required.\n"
                       "\n"
                       "  -f  FUNC  function to pass numbers through. pass -L to see the list.\n"
                       "  -h        display this help and exit\n"
                       "  -L        list all supported functions and exit\n"
                       "  -p  ARG   pass additional arguments to FUNC. this option can be\n"
                       "            given multiple times, and has to be given after -f.\n";
                return EXIT_SUCCESS;
            }
            case 'L': {
                for (const auto& [name, func] : functions)
                    std::cout << name << ":\n\t" << func.description << '\n';

                return EXIT_SUCCESS;
            }
            case 'p': {
                if (auto& func_argv = func_argvs.back(); func_argv.size() == func_argv.capacity())
                    xph::die("too many parameters given to function ",
                             "" /* TODO: print function name */
                    );
                else
                    func_argv.push_back(xph::lexical_cast<char*, double>(optarg));
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

    if (!sel_funcs.empty() && func_argvs.back().size() < sel_funcs.back().min_argc)
        xph::die("too few parameters given to function ", "" /* TODO: print function name */
        );

    std::vector<double> nums;
    for (const auto& arg : std::views::counted(argv, argc))
        nums.push_back(xph::lexical_cast<char*, double>(arg));

    for (const auto&& [func, func_argv] : boost::combine(sel_funcs, func_argvs))
        func.function(func_argv, nums);
    for (const auto& num : nums)
        std::cout << num << '\n';

    return 0;
}

namespace func {
    void acos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::acos(num);
    }

    void asin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::asin(num);
    }

    void atan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::atan(num);
    }

    void cbrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::cbrt(num);
    }

    void ceil([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::ceil(num);
    }

    void cos([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::cos(num);
    }

    void cosh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::cosh(num);
    }

    void exp([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::exp(num);
    }

    void fabs([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::fabs(num);
    }

    void floor([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::floor(num);
    }

    void log([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::log(num);
    }

    void log10([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::log10(num);
    }

    void log2([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::log2(num);
    }

    void pow(std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::pow(num, argv[0]);
    }

    void round([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::round(num);
    }

    void sin([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::sin(num);
    }

    void sinh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::sinh(num);
    }

    void sqrt([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::sqrt(num);
    }

    void tan([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::tan(num);
    }

    void tanh([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::tanh(num);
    }

    void trunc([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        for (auto& num : nums)
            num = ::trunc(num);
    }

    void nonzero([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        std::erase_if(nums, [](double num) { return ::approxzero(num); });
    }

    void max([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { *std::max_element(nums.begin(), nums.end()) };
    }

    void min([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { *std::min(nums.begin(), nums.end()) };
    }

    void sum([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 0.0) };
    }

    void mean([[maybe_unused]] std::span<double> argv, std::vector<double>& nums)
    {
        nums = { std::accumulate(nums.begin(), nums.end(), 0.0) / nums.size() };
    }
} // namespace func
