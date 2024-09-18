#ifndef HEADER_SCRIPTS_CXX_MACROS_
#define HEADER_SCRIPTS_CXX_MACROS_

#define COMMA ,

#define NOOP(...) \
    do {          \
    } while (false)

#define SIMPLE_NAMEOF(x) #x

#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#define DEFINE_DEFAULT_ENUM(NAME, VALS) \
    struct _##NAME##_definition {       \
        enum type { VALS };             \
    };                                  \
    typedef xph::macros::safe_enum<_##NAME##_definition> NAME

#define DEFINE_TYPED_ENUM(TYPE, NAME, VALS) \
    struct _##NAME##_definition {           \
        enum type { VALS };                 \
    };                                      \
    typedef xph::macros::safe_enum<_##NAME##_definition, TYPE> NAME

#define DEFINE_ENUM(NAME, VALS)   \
    struct _##NAME##_definition { \
        enum type { VALS };       \
    };                            \
    typedef xph::macros::safe_enum<_##NAME##_definition, int> NAME

namespace xph::macros {
    template <typename def, typename inner = typename def::type>
    class safe_enum : public def {
        typedef inner type;
        inner val;

    public:
        safe_enum() {}

        safe_enum(type v) : val(v) {}

        type value() const
        {
            return val;
        }

        friend bool operator==(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val == rhs.val;
        }

        friend bool operator!=(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val != rhs.val;
        }

        friend bool operator<(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val < rhs.val;
        }

        friend bool operator<=(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val <= rhs.val;
        }

        friend bool operator>(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val > rhs.val;
        }

        friend bool operator>=(const safe_enum& lhs, const safe_enum& rhs)
        {
            return lhs.val >= rhs.val;
        }
    };

#define STRING_SWITCH(VALUE)                     \
    const std::string STRING_SWITCH_LHS = VALUE; \
    if (false) {}
#define STRING_CASE(VALUE)                                         \
    else if (STRING_SWITCH_LHS == static_cast<std::string>(VALUE)) \
    {
#define STRING_DEFAULT \
    else               \
    {
#define STRING_BREAK }
} // namespace macros

#endif /* ifndef HEADER_SCRIPTS_CXX_MACROS_ */
