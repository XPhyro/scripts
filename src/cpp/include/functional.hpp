#ifndef HEADER_SCRIPTS_CXX_FUNCTIONAL_
#define HEADER_SCRIPTS_CXX_FUNCTIONAL_

#include <functional>

namespace xph {
    template <class Tret, class... Tparams>
    inline constexpr void* address_of(const std::function<Tret(Tparams...)>& func);

    template <class Tret, class... Tparams>
    inline constexpr void* address_of(const std::function<Tret(Tparams...)>& func)
    {
        using func_t = Tret(Tparams...);
        func_t** func_ptr = func.template target<func_t*>();
        return reinterpret_cast<void*>(*func_ptr);
    }

    template <class Tret, class... Tparams, class Tother_ret, class... Tother_params>
    inline constexpr bool
    function_eq(const std::function<Tret(Tparams...)>& func,
                const std::function<Tother_ret(Tother_params...)>& other_func);

    template <class Tret, class... Tparams, class Tother_ret, class... Tother_params>
    inline constexpr bool function_eq(const std::function<Tret(Tparams...)>& func,
                                      const std::function<Tother_ret(Tother_params...)>& other_func)
    {
        return address_of(func) == address_of(other_func);
    }
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_FUNCTIONAL_ */
