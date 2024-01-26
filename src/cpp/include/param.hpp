#ifndef HEADER_SCRIPTS_CXX_PARAM_
#define HEADER_SCRIPTS_CXX_PARAM_

namespace xph {
    template <typename Treturn, typename... Ts>
    inline constexpr Treturn coalesce(const Ts&... args)
    {
        Treturn ret{};

        ([&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(args);
            return condition;
        }() ||
         ...);

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn reverse_coalesce(const Ts&... args)
    {
        Treturn ret{};

        (... || [&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(args);
            return condition;
        }());

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn coalesce_deref(const Ts&... args)
    {
        Treturn ret{};

        ([&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(*args);
            return condition;
        }() ||
         ...);

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn reverse_coalesce_deref(const Ts&... args)
    {
        Treturn ret{};

        (... || [&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(*args);
            return condition;
        }());

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn coalesce_map(const auto& callable, const Ts&... args)
    {
        Treturn ret{};

        ([&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(callable(args));
            return condition;
        }() ||
         ...);

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn reverse_coalesce_map(const auto& callable, const Ts&... args)
    {
        Treturn ret{};

        (... || [&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(callable(args));
            return condition;
        }());

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn coalesce_deref_map(const auto& callable, const Ts&... args)
    {
        Treturn ret{};

        ([&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(callable(*args));
            return condition;
        }() ||
         ...);

        return ret;
    }

    template <typename Treturn, typename... Ts>
    inline constexpr Treturn reverse_coalesce_deref_map(const auto& callable, const Ts&... args)
    {
        Treturn ret{};

        (... || [&]() {
            bool condition = static_cast<bool>(args);
            if (condition)
                ret = static_cast<Treturn>(callable(*args));
            return condition;
        }());

        return ret;
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_PARAM_ */
